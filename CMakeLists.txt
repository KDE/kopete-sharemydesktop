project (kopete-plugin-sharemydesktop)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${TELEPATHY_QT4_INCLUDE_DIR}
)

set (kopete_plugin_sharemydesktop_SRCS
     share-my-desktop-plugin.cpp
     share-my-desktop-gui-client.cpp
)

kde4_add_plugin (kopete_sharemydesktop
                 ${kopete_plugin_sharemydesktop_SRCS}
)

target_link_libraries (kopete_sharemydesktop
                       kopetetelepathy
                       ${TELEPATHY_QT4_LIBRARIES}
                       ${KDE4_KDEUI_LIBS}
                       ${KOPETE_LIBRARIES}
)

install (TARGETS
         kopete_sharemydesktop
         DESTINATION
         ${PLUGIN_INSTALL_DIR}
)

install (FILES
         kopete_sharemydesktop.desktop
         DESTINATION
         ${SERVICES_INSTALL_DIR}
)

install (FILES
         share-my-desktop-chat-ui.rc
         DESTINATION
         ${DATA_INSTALL_DIR}/kopete_sharemydesktop
)

