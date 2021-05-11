/*
 *  Kaidan - A user-friendly XMPP client for every device!
 *
 *  Copyright (C) 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  Copyright (C) 2016-2019 Kaidan developers and contributors
 *  (see the LICENSE file of Kaidan for a full list of copyright authors)
 *
 *  Kaidan is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  In addition, as a special exception, the author of Kaidan gives
 *  permission to link the code of its release with the OpenSSL
 *  project's "OpenSSL" library (or with modified versions of it that
 *  use the same license as the "OpenSSL" library), and distribute the
 *  linked executables. You must obey the GNU General Public License in
 *  all respects for all of the code used other than "OpenSSL". If you
 *  modify this file, you may extend this exception to your version of
 *  the file, but you are not obligated to do so.  If you do not wish to
 *  do so, delete this exception statement from your version.
 *
 *  Kaidan is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kaidan.  If not, see <http://www.gnu.org/licenses/>.
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
                        case QrCodeContent.Binary:
                            return i18n("<binary data>");
                        default:
                            return resultSheet.tag.text;
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
            Controls.Button {
                text: i18n("Cancel")
                onClicked: resultSheet.close()
                Layout.fillWidth: true
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
        onScanningSucceeded: {
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
