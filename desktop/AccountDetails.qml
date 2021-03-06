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
    visible: true
    minimumWidth: grid.implicitWidth + 50
    minimumHeight: rootPane.implicitHeight + footer.height
    title: qsTr("Account Details")
    modality: Qt.NonModal
    flags: Qt.Dialog

    property QtObject account: null

    Pane {
        id: rootPane
        focus: true
        anchors.fill: parent
        implicitHeight: walletType.implicitHeight + 10 + grid.implicitHeight

        Label {
            id: walletType
            width: parent.width
            text: {
                if (root.account.isSingleAddressAccount)
                    return qsTr("This account is a single-address wallet.")

                 // ok we only have one more type so far, so this is rather simple...
                return qsTr("This account is a simple multiple-address wallet.")
            }
            wrapMode: Text.WordWrap
        }
        GridLayout {
            id: grid
            columns: 2
            anchors.top: walletType.bottom
            width: root.width - 20


            Label { text: qsTr("Name") + ":" }
            FloweeTextField {
                text: root.account.name
                focus: true
                Layout.fillWidth: true
                onAccepted:  root.account.name = text
            }
            Label {
                text: qsTr("Balance unconfirmed") + ":"
            }
            BitcoinAmountLabel {
                value: root.account.balanceUnconfirmed
                colorize: false
            }
            Label {
                text: qsTr("Balance immature") + ":"
            }
            BitcoinAmountLabel {
                value: root.account.balanceImmature
                colorize: false
            }
            Label {
                text: qsTr("Balance other") + ":"
            }
            BitcoinAmountLabel {
                value: root.account.balanceConfirmed
                colorize: false
            }
            Label {
                text: qsTr("Available coins") + ":"
            }
            Label {
                text: root.account.unspentOutputCount
            }
            Label {
                text: qsTr("Historical coins") + ":"
            }
            Label {
                text: root.account.historicalOutputCount
            }
            Label {
                text: qsTr("Sync status") + ":"
            }
            SyncIndicator {
                id: syncIndicator
                accountBlockHeight: root.account === null ? 0 : root.account.lastBlockSynched
            }
            Pane {}
            Label {
                text: syncIndicator.accountBlockHeight + " / " + syncIndicator.globalPos
            }
        }
        Keys.onPressed: {
            if (event.key === Qt.Key_Escape) {
                root.visible = false;
                event.accepted = true
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
