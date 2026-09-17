/*
 *  SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.qrca 1.0 as Qrca

Kirigami.ScrollablePage {
    id: page
    title: i18nc("@title:window", "History")

    actions: [
        Kirigami.Action {
            text: i18nc("@action:intoolbar Clear history", "Clear")
            icon.name: "edit-clear-history-symbolic"
            enabled: Qrca.HistoryModel.count > 0
            onTriggered: {
                Qrca.HistoryModel.clear();
            }
        }
    ]

    Connections {
        target: Qrca.HistoryModel
        function onCountChanged() {
            if (Qrca.HistoryModel.count === 0) {
                page.Kirigami.PageStack.pop();
            }
        }
    }

    ListView {
        id: list
        model: Qrca.HistoryModel

        delegate: Controls.ItemDelegate {
            id: delegate
            required property int index
            required text
            required property Qrca.QrCodeContent content

            width: list.width
            highlighted: ListView.isCurrentItem
            icon.name: {
                // TODO More icons.
                switch (content.contentType) {
                case Qrca.QrCodeContent.WifiSetting:
                    return "network-wireless";
                }
                return "";
            }
            onClicked: {
                resultSheet.tag = delegate.content;
                resultSheet.open();
            }

            Keys.onEnterPressed: animateClick()
            Keys.onReturnPressed: animateClick()

            Keys.onDeletePressed: deleteButton.animateClick()

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                    source: delegate.icon.name
                    selected: delegate.highlighted
                    visible: valid
                }

                Controls.Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: deleteButton.implicitHeight
                    text: delegate.text
                    color: delegate.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                    wrapMode: Text.Wrap
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 3
                    textFormat: Text.PlainText
                    Accessible.ignored: true
                }

                Controls.ToolButton {
                    id: deleteButton
                    text: i18nc("@action:button Remove history entry", "Remove")
                    display: Controls.ToolButton.IconOnly
                    icon.name: "edit-delete-symbolic"
                    focusPolicy: Qt.NoFocus
                    visible: delegate.hovered || delegate.highlighted || activeFocus
                    onClicked: {
                        Qrca.HistoryModel.removeAt(delegate.index);
                    }

                    Controls.ToolTip.text: deleteButton.text
                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Controls.ToolTip.visible: deleteButton.hovered
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            icon.name: "edit-none"
            text: i18nc("placeholder", "History empty")
            visible: list.count === 0
        }
    }

    ResultSheet {
        id: resultSheet
        showBarcode: true
    }
}
