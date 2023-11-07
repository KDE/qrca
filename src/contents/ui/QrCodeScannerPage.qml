/*
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import QtMultimedia
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Layouts 1.3
import org.kde.prison.scanner 1.0 as Prison

import org.kde.qrca 1.0

Kirigami.Page {
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    title: qsTr("Scan QR code")
    actions: [
        Kirigami.Action {
            icon.name: checked ? "flashlight-off" : "flashlight-on"
            text: i18n("Light")
            checkable: true
            checked: camera.flash.mode == Camera.FlashVideoLight
            visible: camera.flash.supportedModes.length > 1
            onTriggered: camera.flash.mode = (camera.flash.mode == Camera.FlashVideoLight ? Camera.FlashOff : Camera.FlashVideoLight)
        },
        Kirigami.Action {
            text: i18n("Select Camera")
            visible: devices.videoInputs.length > 1
            icon.name: "camera-video-symbolic"
            onTriggered: cameraSelectorSheet.open()
        }
    ]

    MediaDevices {
        id: devices
    }

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
                    case QrCodeContent.ISBN:
                        return i18n("ISBN found")
                    case QrCodeContent.EAN:
                        return i18n("International Article Number found")
                    case QrCodeContent.WifiSetting:
                        return i18n("Wi-Fi settings found")
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
                        case QrCodeContent.WifiSetting:
                            return Qrca.wifiName(resultSheet.tag.text);
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
                    case QrCodeContent.Url:
                        return i18n("Open")
                    case QrCodeContent.VCard:
                        return i18n("Save contact")
                    case QrCodeContent.OtpToken:
                        return i18n("Open OTP client")
                    case QrCodeContent.EAN:
                        return i18n("Open Food Facts")
                    case QrCodeContent.ISBN:
                        return i18n("Wikipedia Book Sources")
                    case QrCodeContent.TransportTicket:
                        return i18n("Open KDE Itinerary")
                    case QrCodeContent.HealthCertificate:
                        return i18n("Open in Vakzination")
                    case QrCodeContent.WifiSetting:
                        return i18n("Connect")
                    }
                }
                onClicked: {
                    switch (resultSheet.tag.contentType) {
                    case QrCodeContent.Url:
                       Qt.openUrlExternally(resultSheet.tag.text)
                       break
                    case QrCodeContent.VCard:
                         Qrca.saveVCard(resultSheet.tag.text)
                        break
                    case QrCodeContent.OtpToken:
                        Qt.openUrlExternally(resultSheet.tag.text)
                        break
                    case QrCodeContent.EAN:
                        Qt.openUrlExternally("https://world.openfoodfacts.org/product/" + resultSheet.tag.text)
                        break;
                    case QrCodeContent.ISBN:
                        Qt.openUrlExternally("https://en.wikipedia.org/wiki/Special:BookSources?isbn=" + resultSheet.tag.text)
                        break;
                    case QrCodeContent.TransportTicket:
                        Qrca.openInApplication(resultSheet.tag, "org.kde.itinerary");
                        break;
                    case QrCodeContent.HealthCertificate:
                        Qrca.openInApplication(resultSheet.tag, "org.kde.vakzination");
                    case QrCodeContent.WifiSetting:
                        Qrca.connectToWifi(resultSheet.tag.text);
                    }
                    resultSheet.close()
                }
                visible: {
                    switch (resultSheet.tag.contentType) {
                    case QrCodeContent.Binary:
                    case QrCodeContent.Text:
                        return false;
                    case QrCodeContent.TransportTicket:
                        return Qrca.hasApplication("org.kde.itinerary");
                    case QrCodeContent.HealthCertificate:
                        return Qrca.hasApplication("org.kde.vakzination");
                    case QrCodeContent.WifiSetting:
                        return Qrca.canConnectToWifi();
                    }
                    return true;
                }
                icon.name: {
                    switch (resultSheet.tag.contentType) {
                    case QrCodeContent.VCard:
                        return "document-save";
                    case QrCodeContent.OtpToken:
                        return "document-encrypt";
                    case QrCodeContent.TransportTicket:
                        return Qrca.applicationIconName("org.kde.itinerary");
                    case QrCodeContent.HealthCertificate:
                        return Qrca.applicationIconName("org.kde.vakzination");
                    case QrCodeContent.WifiSetting:
                        return "network-wireless";
                    }
                    return "internet-services"
                }

                Layout.fillWidth: true
            }
            Controls.Button {
                text: i18n("Copy to clipboard")
                icon.name: "edit-copy-symbolic"
                onClicked: {
                    Qrca.copyToClipboard(resultSheet.tag)
                    resultSheet.close()
                }
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
            model: devices.videoInputs
            implicitWidth: Kirigami.Units.gridUnit * 20

            delegate: Controls.ItemDelegate {
                text: modelData.description
                width: ListView.view.width
                onClicked: {
                    camera.cameraDevice = modelData;
                    camera.start();
                    cameraSelectorSheet.close();
                }
            }
        }
    }

    VideoOutput {
        id: viewfinder
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop
    }

    Prison.VideoScanner {
        id: scannerFilter

        onResultContentChanged: result => {
            if (!result.hasContent) {
                return
            }

            resultSheet.tag = Qrca.resultContent(result)
            if (!resultSheet.sheetOpen) {
                resultSheet.open()
            }
        }
        videoSink: viewfinder.videoSink
    }

    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            active: true
        }
        videoOutput: viewfinder
    }

}
