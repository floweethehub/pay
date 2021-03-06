/*
 * This file is part of the Flowee project
 * Copyright (C) 2020-2021 Tom Zander <tom@flowee.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.11
import QtQuick.Controls 2.11
import QtQuick.Layouts 1.11

ApplicationWindow {
    id: root
    visible: false
    minimumWidth: 300
    minimumHeight: 400
    width: 500
    height: 600
    title: qsTr("Peers (%1)", "", net.peers.length).arg(net.peers.length)
    modality: Qt.NonModal
    flags: Qt.Dialog


    ListView {
        id: listView
        model: net.peers
        clip: true

        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_Escape) {
                root.visible = false;
                event.accepted = true
            }
        }

        delegate: Rectangle {
            width: listView.width
            height: peerPane.height + 12
            color: index % 2 === 0 ? secondRow.palette.button : secondRow.palette.base
            ColumnLayout {
                id: peerPane
                width: listView.width - 20
                x: 10
                y: 6

                Label {
                    text: modelData.userAgent
                }
                Label {
                    text: qsTr("Address:", "network address (IP)") + " " + modelData.address
                }
                RowLayout {
                    height: secondRow.height
                    Label {
                        id: secondRow
                        text: qsTr("Start-height: %1").arg(modelData.startHeight)
                    }
                    Label {
                        text: qsTr("ban-score: %1").arg(modelData.banScore)
                    }
                }
                Label {
                    id : accountName
                    font.bold: true
                    text: {
                        var id = modelData.segmentId;
                        var accounts = portfolio.accounts;
                        for (var i = 0; i < accounts.length; ++i) {
                            if (accounts[i].id === id)
                                return qsTr("Peer for account: %1").arg(accounts[i].name);
                        }
                        return ""; // not peered yet
                    }
                    visible: text !== ""
                }
            }
        }
    }

    footer: Item {
        width: parent.width
        height: closeButton.height + 20

        Button {
            id: closeButton
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            text: qsTr("Close")
            onClicked: root.visible = false
        }
    }
}
