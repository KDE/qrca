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
import org.kde.prison 1.0 as Prison

import QtQuick.Dialogs 1.2

Kirigami.ScrollablePage {
    title: i18n("Create QR-Code")

    Loader {
        id: shareSheetLoader

        active: false
        source: "ShareSheet.qml"
    }

    FileDialog {
        id: fileDialog
        selectExisting: false
        defaultSuffix: "png"
        nameFilters: [ i18n("Image files (*.png *.jpg)"), i18n("All files (*)") ]
    }

    actions.contextualActions: [
        Kirigami.Action {
            text: i18n("Save")
            icon.name: "document-save"
            enabled: inputText.length > 0
            onTriggered: {
                fileDialog.accepted.connect(() => {
                    codeView.grabToImage((result) => {
                         const s = result.saveToFile(fileDialog.fileUrl.toString().replace("file://", ""))
                         if (!s) {
                             showPassiveNotification(i18n("QR-Code could not be saved"))
                         }

                         fileDialog.accepted.disconnect()
                    })
                })
                fileDialog.open()
            }
        },
        Kirigami.Action {
            text: i18n("Share")
            icon.name: "document-share"
            visible: Qt.platform.os !== "android" // Purpose doesn't work properly on Android
            enabled: inputText.length > 0
            onTriggered: {
                shareSheetLoader.active = true
                codeView.grabToImage((result) => {
                    const saveLocation = Qrca.newQrCodeSaveLocation()
                    const s = result.saveToFile(saveLocation)
                    if (!s) {
                        showPassiveNotification(i18n("QR-Code could not be saved to temporary location"))
                    }

                    shareSheetLoader.item.url = saveLocation
                    shareSheetLoader.item.open()
                })
            }
        }
    ]

    ColumnLayout {
        spacing: 50

        Prison.Barcode {
            id: codeView
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.preferredHeight: width
            barcodeType: Prison.Barcode.QRCode
            content: inputText.text.length > 0 ? inputText.text: "https://www.kde.org"
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
