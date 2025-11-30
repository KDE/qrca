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

import QtQuick.Dialogs

Kirigami.ScrollablePage {
    title: i18nc("@title:window", "Create a QR Code")

    FileDialog {
        id: fileDialog
        defaultSuffix: "png"
        nameFilters: [i18n("Image files (*.png *.jpg)"), i18n("All files (*)")]
        fileMode: FileDialog.SaveFile
    }

    actions: [
        Kirigami.Action {
            text: i18nc("@action:intoolbar", "Save")
            icon.name: "document-save"
            enabled: inputText.length > 0
            onTriggered: {
                fileDialog.accepted.connect(() => {
                    codeView.grabToImage(result => {
                        const s = result.saveToFile(fileDialog.selectedFile);
                        if (!s) {
                            showPassiveNotification(i18n("QR code could not be saved."));
                        }

                        fileDialog.accepted.disconnect();
                    });
                });
                fileDialog.open();
            }
        },
        Kirigami.Action {
            text: i18nc("@action:intoolbar", "Copy")
            icon.name: "edit-copy"
            enabled: inputText.length > 0
            onTriggered: {
                codeView.grabToImage(result => {
                    Qrca.copyImageToClipboard(result.image);
                });
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
            content: inputText.text.length > 0 ? inputText.text : "https://www.kde.org"
        }

        Kirigami.ActionTextField {
            id: inputText
            placeholderText: i18nc("@info:placeholder", "Enter text…")
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
