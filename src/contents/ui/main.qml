/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.1
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.0 as Controls
import org.kde.qrca 1.0

Kirigami.ApplicationWindow {
    id: root

    title: i18n("QR Code Scanner")

    function passiveNotification(text) {
        showPassiveNotification(text, "long")
    }

    Component.onCompleted: {
        Qrca.passiveNotificationRequested.connect(passiveNotification)
        if (Qrca.encodeText) {
            qrCodeEncoderAction.trigger()
        } else {
            qrCodeScannerAction.trigger()
        }
    }

    Kirigami.PagePool {
        id: mainPagePool
    }

    readonly property list<Kirigami.PagePoolAction> navigationActions: [
        Kirigami.PagePoolAction {
            id: qrCodeScannerAction
            text: i18n("Scan")
            icon.name: "camera-photo-symbolic"
            pagePool: mainPagePool
            page: "QrCodeScannerPage.qml"
        },
        Kirigami.PagePoolAction {
            id: qrCodeEncoderAction
            text: i18n("Create")
            icon.name: "document-new-symbolic"
            pagePool: mainPagePool
            page: "QrCodeEncoderPage.qml"
        },
        Kirigami.PagePoolAction {
            text: i18n("About")
            icon.name: "help-feedback"
            pagePool: mainPagePool
            page: "AboutPage.qml"
        }
    ]

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("QR-Code Scanner")
        isMenu: root.wideScreen
        actions: root.wideScreen ? navigationActions : []
        enabled: false
    }

    footer: Kirigami.NavigationTabBar {
        visible: true || !root.wideScreen
        actions: navigationActions
    }

    contextDrawer: Kirigami.ContextDrawer {}
}
