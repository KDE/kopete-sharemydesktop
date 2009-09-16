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

#include "share-my-desktop-plugin.h"

#include "share-my-desktop-gui-client.h"

#include <KAboutData>
#include <KComponentData>
#include <KDebug>
#include <KPluginFactory>
#include <KPluginInfo>

#include <kopetechatsessionmanager.h>
#include <kopetechatsession.h>
#include <kopeteprotocol.h>
#include <ui/kopeteview.h>
#include <ui/kopeteviewplugin.h>

K_PLUGIN_FACTORY(ShareMyDesktopPluginFactory, registerPlugin<ShareMyDesktopPlugin>(); )
K_EXPORT_PLUGIN(ShareMyDesktopPluginFactory("kopete_sharemydesktop"))

ShareMyDesktopPlugin::ShareMyDesktopPlugin(QObject *parent, const QVariantList &args)
 : Kopete::Plugin(ShareMyDesktopPluginFactory::componentData(), parent)
{
    kDebug();

    Q_UNUSED(args);

    // Connect to the signal emitted when a new Chat Session is started.
    connect(Kopete::ChatSessionManager::self(),
            SIGNAL(viewCreated(KopeteView*)),
            SLOT(onViewCreated(KopeteView*)));
}

ShareMyDesktopPlugin::~ShareMyDesktopPlugin()
{
    kDebug();
}

void ShareMyDesktopPlugin::onViewCreated(KopeteView *view)
{
    if (view->plugin()->pluginInfo().pluginName() != QString::fromLatin1("kopete_chatwindow")) {
        // Only conventional text chat windows are supported by this plugin (e.g. not email ones).
        return;
    }

    // Get the chat session
    Kopete::ChatSession *session = view->msgManager();

    if (!session) {
        kWarning() << "Null chat session. Aborting.";
        return;
    }

    // Check that this chat session is using the Telepathy protocol (since this plugin only works
    // with the Telepathy protocol.
    if (session->protocol()->componentData().componentName() != "kopete_telepathy") {
        kDebug() << "Chat is not using Telepathy protocol. Do not use this plugin.";
        return;
    }

    kDebug() << "Chat is using Telepathy protocol :):):)";

    new ShareMyDesktopGuiClient(session);
}


#include "share-my-desktop-plugin.moc"

