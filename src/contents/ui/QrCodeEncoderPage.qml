/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import QtMultimedia 5.9
import org.kde.kirigami 2.7 as Kirigami
import QtQuick.Layouts 1.3
import org.kde.qrca 1.0

Kirigami.ScrollablePage {
    title: i18n("Create QR-Code")

    Loader {
        id: shareSheetLoader

        active: false
        source: "ShareSheet.qml"
    }

    actions.contextualActions: [
        Kirigami.Action {
            property string path
            text: i18n("Save")
            icon.name: "document-save"
            onTriggered: {
                path = "file://" + Qrca.save(codeView.source)
                showPassiveNotification(i18n("Saved image to " + path), "long", "Open Externally", function() {Qt.openUrlExternally(path)})
            }
        },
        Kirigami.Action {
            text: i18n("Share")
            icon.name: "document-share"
            visible: Qt.platform.os !== "android" // Purpose doesn't work properly on Android
            onTriggered: {
                shareSheetLoader.active = true
                shareSheetLoader.item.url = Qrca.save(codeView.source)
                shareSheetLoader.item.open()
            }
        }
    ]

    ColumnLayout {
        spacing: 50

        Kirigami.Icon {
            id: codeView
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16

            implicitHeight: width
            source: Qrca.encode(inputText.text.length > 0 ? inputText.text: " ", width)
        }

        Kirigami.ActionTextField {
            id: inputText
            placeholderText: "Enter text"
            text: Qrca.encodeText ? Qrca.encodeText : ""
            Layout.fillWidth: true
            rightActions: [
                Kirigami.Action {
                    icon.name: "edit-clear"
                    enabled: inputText.text.length > 0
                    onTriggered: inputText.text = ""
                }
            ]
        }
    }
}
