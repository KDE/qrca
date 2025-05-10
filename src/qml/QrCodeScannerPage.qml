/*
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2016-2019 Kaidan developers and contributors (see the LICENSE file of Kaidan for a full list of copyright authors)
 *  SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *  SPDX-FileCopyrightText: 2025 Salvo 'LtWorf' Tomaselli <ltworf@debian.org>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtCore
import QtQml // for Connections.enabled
import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import QtMultimedia
import org.kde.kirigami as Kirigami
import QtQuick.Layouts 1.3
import org.kde.prison.scanner 1.0 as Prison
import QtQuick.Dialogs
import org.kde.qrca 1.0

Kirigami.Page {
    id: scanner
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    property bool windowActive: Controls.ApplicationWindow.window.active

    onWindowActiveChanged: {
        // Deactivate camera on mobile, if the window is inactive
        if (! Kirigami.Settings.isMobile)
            return

        if (scanner.windowActive) {
            camera.start()
            deactivateCamera.running = false
        } else {
            deactivateCamera.running = true
        }
    }

    Timer {
        id: deactivateCamera
        interval: 1000 * 5
        repeat: false
        running: false
        triggeredOnStart: false
        onTriggered: {
            camera.stop()
        }
    }

    title: Qrca.wifiMode ? i18n("Scan for Wifi code") : i18n("Scan QR code")
    actions: [
        Kirigami.Action {
            icon.name: checked ? "flashlight-off" : "flashlight-on"
            text: i18n("Light")
            checkable: true
            checked: camera.torchMode == Camera.TorchOn
            visible: camera.cameraDevice, camera.isTorchModeSupported(Camera.TorchOn)
            onTriggered: camera.torchMode = (camera.torchMode == Camera.TorchOn ? Camera.TorchOff : Camera.TorchOn)
        },
        Kirigami.Action {
            text: i18n("Select Camera")
            visible: devices.videoInputs.length > 1
            icon.name: "camera-video-symbolic"
            onTriggered: cameraSelectorSheet.open()
        },
        Kirigami.Action {
            icon.name: "document-open"
            text: i18n("Open Image")
            onTriggered: {
                openFileDialog.open();
                camera.active = false;
            }
        }
    ]

    CameraPermission {
        id: permission
        onStatusChanged: {
            if (status == Qt.PermissionStatus.Granted) {
                camera.start();
            }
        }
    }

    MediaDevices {
        id: devices
    }

    function asLink(text) {
        return "<a href='" + text + "'>" + text + "</a>";
    }

    Kirigami.OverlaySheet {
        id: resultSheet

        property var tag
        onTagChanged: resultErrorMessage.visible = false

        header: Kirigami.Heading {
            text: {
                switch (resultSheet.tag?.contentType) {
                case QrCodeContent.Text:
                    return i18n("Text found");
                case QrCodeContent.Url:
                    return i18n("URL found");
                case QrCodeContent.VCard:
                    return i18n("Contact found");
                case QrCodeContent.OtpToken:
                    return i18n("OTP URI found");
                case QrCodeContent.Binary:
                    return i18n("Binary data found");
                case QrCodeContent.HealthCertificate:
                    return i18n("Health certificate found");
                case QrCodeContent.TransportTicket:
                    return i18n("Transport ticket found");
                case QrCodeContent.ISBN:
                    return i18n("ISBN found");
                case QrCodeContent.EAN:
                    return i18n("International Article Number found");
                case QrCodeContent.WifiSetting:
                    return i18n("Wi-Fi settings found");
                default:
                    return "";
                }
            }
        }

        ColumnLayout {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 20

            Kirigami.InlineMessage {
                id: resultErrorMessage
                Layout.fillWidth: true
                type: Kirigami.MessageType.Error
                visible: false
            }

            Controls.Label {
                Layout.maximumWidth: Kirigami.Units.gridUnit * 20
                Layout.fillWidth: true
                text: {
                    switch (resultSheet.tag?.contentType) {
                    case QrCodeContent.VCard:
                        return Qrca.getVCardName(resultSheet.tag.text);
                    case QrCodeContent.WifiSetting:
                        return Qrca.wifiName(resultSheet.tag.text);
                    default:
                        return resultSheet.tag?.isPlainText ? resultSheet.tag.text : i18n("<binary data>");
                    }
                }
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
            }
        }

        Connections {
            target: Qrca
            enabled: resultSheet.opened && resultSheet.tag?.contentType === QrCodeContent.WifiSetting
            function onWifiConnected(): void {
                if (Qrca.wifiMode) {
                    Qt.quit();
                } else {
                    resultSheet.close();
                }
            }
            function onWifiConnectionFailed(msg): void {
                resultErrorMessage.text = msg;
                resultErrorMessage.visible = true;
            }
        }

        footer: RowLayout {
            RowLayout {
                spacing: 0
                visible: wifiBusyIndicator.running

                Controls.BusyIndicator {
                    id: wifiBusyIndicator
                    running: resultSheet.tag?.contentType === QrCodeContent.WifiSetting && Qrca.connectingToWifi
                }

                Controls.Label {
                    text: i18nc("Connecting to Wifi", "Connecting…")
                }
            }

            Controls.Button {
                text: {
                    switch (resultSheet.tag?.contentType) {
                    case QrCodeContent.Url:
                        return i18n("Open");
                    case QrCodeContent.VCard:
                        return i18n("Save contact");
                    case QrCodeContent.OtpToken:
                        return i18n("Open OTP client");
                    case QrCodeContent.EAN:
                        return i18n("Open Food Facts");
                    case QrCodeContent.ISBN:
                        return i18n("Wikipedia Book Sources");
                    case QrCodeContent.TransportTicket:
                        return i18n("Open KDE Itinerary");
                    case QrCodeContent.HealthCertificate:
                        return i18n("Open in Vakzination");
                    case QrCodeContent.WifiSetting:
                        return i18n("Connect");
                    default:
                        return "";
                    }
                }
                onClicked: {
                    resultErrorMessage.visible = false;

                    switch (resultSheet.tag.contentType) {
                    case QrCodeContent.Url:
                        Qt.openUrlExternally(resultSheet.tag.text);
                        break;
                    case QrCodeContent.VCard:
                        Qrca.saveVCard(resultSheet.tag.text);
                        break;
                    case QrCodeContent.OtpToken:
                        Qt.openUrlExternally(resultSheet.tag.text);
                        break;
                    case QrCodeContent.EAN:
                        Qt.openUrlExternally("https://world.openfoodfacts.org/product/" + resultSheet.tag.text);
                        break;
                    case QrCodeContent.ISBN:
                        Qt.openUrlExternally("https://en.wikipedia.org/wiki/Special:BookSources?isbn=" + resultSheet.tag.text);
                        break;
                    case QrCodeContent.TransportTicket:
                        Qrca.openInApplication(resultSheet.tag, "org.kde.itinerary");
                        break;
                    case QrCodeContent.HealthCertificate:
                        Qrca.openInApplication(resultSheet.tag, "org.kde.vakzination");
                        break;
                    case QrCodeContent.WifiSetting:
                        Qrca.connectToWifi(resultSheet.tag.text);
                        return; // Wait for connection to be established before closing.
                    }
                    resultSheet.close();
                }
                visible: {
                    switch (resultSheet.tag?.contentType) {
                    case QrCodeContent.Binary:
                    case QrCodeContent.Text:
                        return false;
                    case QrCodeContent.TransportTicket:
                        return Qrca.hasApplication("org.kde.itinerary");
                    case QrCodeContent.HealthCertificate:
                        return Qrca.hasApplication("org.kde.vakzination");
                    case QrCodeContent.WifiSetting:
                        return Qrca.canConnectToWifi() && !Qrca.connectingToWifi;
                    }
                    return true;
                }
                icon.name: {
                    switch (resultSheet.tag?.contentType) {
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
                    return "internet-services";
                }

                Layout.fillWidth: true
            }
            Controls.Button {
                text: i18n("Copy to clipboard")
                icon.name: "edit-copy-symbolic"
                onClicked: {
                    Qrca.copyToClipboard(resultSheet.tag);
                    resultSheet.close();
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
                    scanner_settings.camera_id = modelData.id
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
                return;
            }

            const resultContent = Qrca.resultContent(result);
            if (Qrca.wifiMode && resultContent.contentType !== QrCodeContent.WifiSetting) {
                return;
            }

            resultSheet.tag = resultContent;
            if (!resultSheet.sheetOpen) {
                resultSheet.open();
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

    Kirigami.PlaceholderMessage {
        text: camera.errorString
        visible: camera.error != Camera.NoError
        anchors.fill: parent
    }

    Settings {
        id: scanner_settings
        property string camera_id: ""
    }

    Component.onCompleted: {
        if (permission.status == Qt.PermissionStatus.Undetermined)
            permission.request();

        // Try to restore the previously selected camera
        for (let i = 0; i < devices.videoInputs.length; i++) {
            if (devices.videoInputs[i].id == scanner_settings.camera_id) {
                camera.cameraDevice = devices.videoInputs[i];
            }
        }
    }

    FileDialog {
        id: openFileDialog
        title: i18nc("@title:window", "Choose an image file")
        nameFilters: [i18nc("Name filter for Image files", "Image files (*.jpeg *.jpg *.jxl *.png)")]
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        onAccepted: {
            selectedImage.source = openFileDialog.currentFile;
            viewfinder.visible = false;
            selectedImage.visible = true;
            camera.active = false;

            const result = Qrca.scanImage(openFileDialog.currentFile);
            if (result.hasContent) {
                const resultContent = Qrca.resultContent(result);
                resultSheet.tag = resultContent;
                resultSheet.open();
            } else {
                showPassiveNotification(i18n("No QR code found in the image"), "long");
                selectedImage.source = "";
                selectedImage.visible = false;
                viewfinder.visible = true;
                camera.active = true;
            }
        }
        onRejected: {
            camera.active = true;
        }
    }

    Image {
        id: selectedImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        visible: false

        Controls.Button {
            anchors {
                top: parent.top
                right: parent.right
                margins: Kirigami.Units.largeSpacing
            }
            icon.name: "window-close"
            onClicked: {
                selectedImage.source = "";
                selectedImage.visible = false;
                viewfinder.visible = true;
                camera.active = true;
            }
        }
    }
}
