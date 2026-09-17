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
        if (!Kirigami.Settings.isMobile)
            return;
        if (scanner.windowActive) {
            camera.start();
            deactivateCamera.running = false;
        } else {
            deactivateCamera.running = true;
        }
    }

    onIsCurrentPageChanged: {
        if (isCurrentPage) {
            camera.start();
        } else {
            camera.stop();
        }
    }

    Timer {
        id: deactivateCamera
        interval: 1000 * 5
        repeat: false
        running: false
        triggeredOnStart: false
        onTriggered: {
            camera.stop();
        }
    }

    title: Qrca.wifiMode ? i18n("Scan a Wi-Fi Code") : i18n("Scan a QR Code")
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
        },
        Kirigami.Action {
            icon.name: "view-history-symbolic"
            text: i18nc("@action:intoolbar", "View History")
            enabled: HistoryModel.count > 0
            visible: !Qrca.wifiMode
            onTriggered: {
                scanner.Kirigami.PageStack.push(Qt.resolvedUrl("HistoryPage.qml"));
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

    ResultSheet {
        id: resultSheet
    }

    Loader {
        id: shareSheetLoader
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
                    scanner_settings.camera_id = modelData.id;
                }
            }
        }
    }

    VideoOutput {
        id: viewfinder
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop
        visible: camera.error === Camera.NoError

        Component.onCompleted: {
            // TODO Set it directly once we depend on Qt 6.9
            if (viewfinder.hasOwnProperty("endOfStreamPolicy")) {
                viewfinder.endOfStreamPolicy = VideoOutput.KeepLastFrame;
            }
        }
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
            HistoryModel.add(resultContent);
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
        title: i18nc("@title:window", "Choose an Image File")
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
                HistoryModel.add(resultContent);
                resultSheet.open();
            } else {
                showPassiveNotification(i18n("No QR code found in the image."), "long");
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
