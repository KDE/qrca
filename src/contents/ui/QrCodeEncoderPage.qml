/*
 *  Copyright (C) 2019 Jonah Brüchert <jbb@kaidan.im>
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

        ShareSheet {
            id: shareSheet
            title: i18n("Share QR-Code")
        }
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
                shareSheet.url = Qrca.save(codeView.source)
                shareSheet.open()
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
