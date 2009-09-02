/*
 * This file is part of Kopete
 *
 * Copyright (C) 2009 Collabora Ltd. <info@collabora.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "share-my-desktop-gui-client.h"

#include "share-my-desktop-plugin.h"

#include <KopeteTelepathy/telepathychatsession.h>
#include <KopeteTelepathy/telepathycontact.h>

#include <kopeteaccount.h>
#include <kopetechatsession.h>

#include <KAction>
#include <KActionCollection>
#include <KDebug>
#include <KGenericFactory>
#include <KShortcut>

#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ReferencedHandles>

ShareMyDesktopGuiClient::ShareMyDesktopGuiClient(Kopete::ChatSession *parent)
 : QObject(parent),
   KXMLGUIClient(parent),
   m_chatSession(parent)
{
    kDebug();

    // Set the component data to use as the plugin's
    setComponentData(KGenericFactory<ShareMyDesktopPlugin>::componentData());

    // If the chat session is invalid or empty, this instance should never have happened, but best
    // to be safe and check again anyway.
    if (!m_chatSession || m_chatSession->members().isEmpty()) {
        kWarning() << "Invalid Chat Session.";
        deleteLater();
    }

    // Same for if it is a telepathy chat.
    TelepathyChatSession *tpChatSession = qobject_cast<TelepathyChatSession*>(m_chatSession);

    if (!tpChatSession) {
        kWarning() << "Not a Telepathy Chat Session.";
        deleteLater();
    }

    // Add a KAction for Share My Desktop.
    // FIXME: Are the label and accelerator and shortcut here OK?
    KAction *shareMyDesktop = new KAction(KIcon("krfb"), i18n("S&hare My Desktop"), this);
    actionCollection()->addAction( "shareMyDesktop", shareMyDesktop );
    shareMyDesktop->setShortcut(KShortcut (Qt::CTRL + Qt::Key_D));
    connect(shareMyDesktop, SIGNAL(triggered(bool)), this, SLOT(onShareMyDesktopTriggered()));

    // Set the RC file to use to be the one with our toolbar button in it.
    setXMLFile("share-my-desktop-chat-ui.rc");
}

ShareMyDesktopGuiClient::~ShareMyDesktopGuiClient()
{
    kDebug();
}

void ShareMyDesktopGuiClient::onShareMyDesktopTriggered()
{
    kDebug();

    // When the share-my-desktop button is pressed this slot is called.

    // First check that the Chat Session is valid.
    if (!m_chatSession) {
        kWarning() << "Chat session is no longer valid.";
        return;
    }

    // Get the remote contacts we are chatting with. Sharing a desktop to more than one contact
    // in the same chat is not currently supported.
    // FIXME: Support sharing desktop to everyone in a Multi-User-Chat
    Kopete::ContactPtrList members = m_chatSession->members();

    if (members.size() <= 0) {
        kWarning() << "Invalid number of people in the chat. Aborting share-my-desktop.";
        return;
    }

    if (members.size() > 1) {
        kWarning() << "Share-My-Desktop only supports 1-1 chats at the moment.";
        return;
    }

    // Cast the only member of the chat to a TelepathyContact.
    TelepathyContact *tpContact = qobject_cast<TelepathyContact*>(members.at(0));

    // Check the member really is a telepathy contact.
    if (!tpContact) {
        kWarning() << "Chat member is not a telepathy contact. Aborting share-my-desktop.";
        return;
    }

    // Get the internal Tp::Contact and save it as a member variable.
    m_contact = tpContact->internalContact();

    if (!m_contact) {
        kWarning() << "Internal Contact is null. Aborting share-my-desktop.";
        return;
    }

    // Get the Account on which we are requesting a channel.
    Kopete::Account *kAccount = tpContact->account();

    // Check the account is not null
    if (!kAccount) {
        kWarning() << "Contact's account is not valid. Aborting share-my-desktop.";
        return;
    }

    // Cast to a telepathy account
    TelepathyAccount *tpAccount = qobject_cast<TelepathyAccount*>(kAccount);

    // Check the TelepathyAccount is valid.
    if (!tpAccount) {
        kWarning() << "Account is not a a TelepathyAccount. Aborting share-my-desktop.";
        return;
    }

    // The account is valid. Get its telepathy account and save it as a member variable.
    m_account = tpAccount->account();

    // Check that the internal telepathy account is valid.
    if (!m_account) {
        kWarning() << "Tp::Account is not valid. Aborting share-my-desktop.";
        return;
    }

    // Now we get the Tp::Account ready with the core feature.
    connect(m_account->becomeReady(Tp::Account::FeatureCore),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountReady(Tp::PendingOperation*)));
}

void ShareMyDesktopGuiClient::onAccountReady(Tp::PendingOperation *op)
{
    kDebug();

    if (op->isError()) {
        kWarning() << "Making the Account ready failed:" << op->errorName() << op->errorMessage();
        return;
    }

    // Ensure a rfb stream tube channel to this contact.
    QVariantMap parameters;
    parameters.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
                      TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE);
    parameters.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
                      Tp::HandleTypeContact);
    parameters.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandle"),
                      m_contact->handle().at(0));
    parameters.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE ".Service"),
                      "rfb");

    connect(m_account->ensureChannel(parameters),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onEnsureChannelFinished(Tp::PendingOperation*)));
}

void ShareMyDesktopGuiClient::onEnsureChannelFinished(Tp::PendingOperation *op)
{
    if (op->isError()) {
        kWarning() << "Ensuring channel failed:" << op->errorName() << op->errorMessage();
        return;
    }
}


#include "share-my-desktop-gui-client.moc"

