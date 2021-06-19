/*
 *  Copyright (C) 2019 Jonah Brüchert <jbb@kaidan.im>
 *  Copyright (C) 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *
 *  Qrca is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Qrca is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this project.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import org.kde.kirigami 2.11 as Kirigami
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
            pageStack.layers.push(qrCodeEncoderPage)
        }
    }

    Kirigami.PagePool {
        id: mainPagePool
    }

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("QR-Code Scanner")
        titleIcon: "view-barcode"

        modal: !root.wideScreen
        onEnabledChanged: drawerOpen = enabled && !modal
        onModalChanged: drawerOpen = !modal
        contentItem.implicitWidth: Kirigami.Units.gridUnit * 12

        header: Kirigami.AbstractApplicationHeader {
            topPadding: Kirigami.Units.smallSpacing
            bottomPadding: Kirigami.Units.largeSpacing
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
            Kirigami.Heading {
                text: i18n("Qrca")
            }
        }

        actions: [
            Kirigami.PagePoolAction {
                text: i18n("Scan")
                icon.name: "camera-photo-symbolic"
                pagePool: mainPagePool
                page: "QrCodeScannerPage.qml"
            },
            Kirigami.PagePoolAction {
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
    }

    contextDrawer: Kirigami.ContextDrawer {}

    Component {id: qrCodeScannerPage; QrCodeScannerPage {}}
    Component {id: qrCodeEncoderPage; QrCodeEncoderPage {}}
    Component {id: aboutPage; AboutPage {}}

    pageStack.initialPage: qrCodeScannerPage
}
