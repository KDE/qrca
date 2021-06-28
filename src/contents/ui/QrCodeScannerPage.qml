/*
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import QtMultimedia 5.9
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Layouts 1.3

import org.kde.qrca 1.0

Kirigami.Page {
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    title: qsTr("Scan QR code")
    contextualActions: [
        Kirigami.Action {
            text: i18n("Select Camera")
            visible: QtMultimedia.availableCameras.length > 1
            icon.name: "camera-video-symbolic"
            onTriggered: cameraSelectorSheet.open()
        }
    ]

    function asLink(text) {
        return "<a href='" + text + "'>" + text + "</a>"
    }

    Kirigami.OverlaySheet {
        id: resultSheet

        property var tag

        header: Kirigami.Heading {
            text: {
                switch (resultSheet.tag.contentType) {
                    case QrCodeContent.Text:
                        return i18n("Text found")
                    case QrCodeContent.Url:
                        return i18n("URL found")
                    case QrCodeContent.VCard:
                        return i18n("Contact found")
                    case QrCodeContent.OtpToken:
                        return i18n("OTP URI found")
                    case QrCodeContent.Binary:
                        return i18n("Binary data found")
                    case QrCodeContent.HealthCertificate:
                        return i18n("Health certificate found")
                    case QrCodeContent.TransportTicket:
                        return i18n("Transport ticket found")
                }
            }
        }

        Item {
            implicitWidth: Kirigami.Units.gridUnit * 20
            height: childrenRect.height

            Controls.Label {
                text: {
                    switch(resultSheet.tag.contentType) {
                        case QrCodeContent.VCard:
                            return Qrca.getVCardName(resultSheet.tag.text);
                        default:
                            return resultSheet.tag.isPlainText ? resultSheet.tag.text : i18n("<binary data>");
                    }
                }
                anchors.left: parent.left
                anchors.right: parent.right
                wrapMode: Text.Wrap
            }
        }

        footer: RowLayout {
            Controls.Button {
                text: {
                    switch (resultSheet.tag.contentType) {
                    case QrCodeContent.Binary:
                    case QrCodeContent.Text:
                    case QrCodeContent.HealthCertificate:
                    case QrCodeContent.TransportTicket:
                        return i18n("Copy to clipboard")
                    case QrCodeContent.Url:
                        return i18n("Open")
                    case QrCodeContent.VCard:
                        return i18n("Save contact")
                    case QrCodeContent.OtpToken:
                        return i18n("Open OTP client")
                    }
                }
                onClicked: {
                    switch (resultSheet.tag.contentType) {
                    case QrCodeContent.Binary:
                    case QrCodeContent.Text:
                    case QrCodeContent.HealthCertificate:
                    case QrCodeContent.TransportTicket:
                        Qrca.copyToClipboard(resultSheet.tag)
                        break
                    case QrCodeContent.Url:
                       Qt.openUrlExternally(resultSheet.tag.text)
                       break
                    case QrCodeContent.VCard:
                         Qrca.saveVCard(resultSheet.tag.text)
                        break
                    case QrCodeContent.OtpToken:
                        Qt.openUrlExternally(resultSheet.tag.text)
                        break
                    }
                    resultSheet.close()
                }
                Layout.fillWidth: true
            }
            Controls.Button{
                text: i18n("Save")
                visible: resultSheet.tag.contentType !== QrCodeContent.Binary
                onClicked: {
                    resultSheet.tag.saveCode()
                    resultSheet.close()
                }

            }
            Controls.Button {
                text: i18n("Cancel")
                onClicked: resultSheet.close()
                Layout.fillWidth: true
            }
        }
    }

    Kirigami.OverlaySheet {
        id: cameraSelectorSheet

        header: Kirigami.Heading {
            text: i18n("Select Camera")
        }

        ListView {
            model: QtMultimedia.availableCameras
            delegate: Kirigami.BasicListItem {
                text: modelData.displayName
                onClicked: {
                    camera.deviceId = modelData.deviceId;
                    camera.start();
                    cameraSelectorSheet.close();
                }
            }
        }
    }

    VideoOutput {
        id: viewfinder
        anchors.fill: parent
        source: camera
        autoOrientation: true
        filters: [scannerFilter]
    }

    QrCodeScannerFilter {
        id: scannerFilter
        onScanningSucceeded: (result) => {
            resultSheet.tag = result
            if (!resultSheet.sheetOpen)
                resultSheet.open()
        }
        onUnsupportedFormatReceived: {
            passiveNotification(qsTr("The camera format '%1' is not supported.").arg(format))
        }
    }

    Camera {
        id: camera
        focus {
            focusMode: Camera.FocusContinuous
            focusPointMode: Camera.FocusPointCenter
        }
    }

    Component.onCompleted: {
        scannerFilter.setCameraDefaultVideoFormat(camera);
    }
}
