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

#ifndef KOPETE_PLUGIN_SHAREMYDESKTOP_SHARE_MY_DESKTOP_GUI_CLIENT_H
#define KOPETE_PLUGIN_SHAREMYDESKTOP_SHARE_MY_DESKTOP_GUI_CLIENT_H

#include <KXMLGUIClient>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Contact>

namespace Kopete {
    class ChatSession;
}

namespace Tp {
    class PendingOperation;
}

class ShareMyDesktopGuiClient : public QObject, public KXMLGUIClient
{
    Q_OBJECT

public:
    ShareMyDesktopGuiClient(Kopete::ChatSession *parent = 0);
    ~ShareMyDesktopGuiClient();

private Q_SLOTS:
    void onShareMyDesktopTriggered();
    void onAccountReady(Tp::PendingOperation *op);
    void onEnsureChannelFinished(Tp::PendingOperation *op);

private:
    Kopete::ChatSession *m_chatSession;

    Tp::AccountPtr m_account;
    Tp::ContactPtr m_contact;
};


#endif  // Header guard

