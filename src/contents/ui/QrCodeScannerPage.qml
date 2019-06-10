/*
 *  Kaidan - A user-friendly XMPP client for every device!
 *
 *  Copyright (C) 2016-2019 Kaidan developers and contributors
 *  (see the LICENSE file for a full list of copyright authors)
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

import org.kde.qrskanner 1.0

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

        property string tag
        property bool isLink: false

        ColumnLayout {
            Kirigami.Heading {
                text: i18n("Tag found")
                Layout.fillWidth: true
            }

            Controls.Label {
                text: {
                    if (resultSheet.isLink)
                        i18n("The following tag has been found. Do you want to open the url?")
                    else
                        i18n("The following tag has been found.")
                }
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
            Controls.TextArea {
                visible: !resultSheet.isLink
                text: resultSheet.tag
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                selectByKeyboard: true
                selectByMouse: true
            }
            Controls.Label {
                visible: resultSheet.isLink
                text: asLink(resultSheet.tag)
                Layout.fillWidth: true
            }

            RowLayout {
                Controls.Button {
                    text: i18n("Open")
                    enabled: resultSheet.isLink
                    onClicked: Qt.openUrlExternally(resultSheet.tag)
                    Layout.fillWidth: true
                }
                Controls.Button {
                    text: i18n("Cancel")
                    onClicked: resultSheet.close()
                    Layout.fillWidth: true
                }
            }
        }
    }

	VideoOutput {
		id: viewfinder
		anchors.fill: parent
		source: camera
		autoOrientation: true
	}

	QrCodeScanner {
		camera: camera
		onScanningSucceeded: {
			resultSheet.tag = result
			resultSheet.isLink = qrSkanner.isUrl(result)
			resultSheet.open()
		}
	}

	Camera {
		id: camera
		onAvailabilityChanged: {
			kaidan.qrCodeScanner.startScanning(camera.mediaObject)
		}
	}
}
