/*
 * This file is part of the Flowee project
 * Copyright (C) 2020 Tom Zander <tomz@freedommail.ch>
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
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Flowee.org.pay 1.0

import "./ControlColors.js" as ControlColors

ApplicationWindow {
    id: mainWindow
    visible: true
    width: Flowee.windowWidth == -1 ? 600 : Flowee.windowWidth
    height: Flowee.windowHeight == -1 ? 400 : Flowee.windowHeight
    minimumWidth: 300
    minimumHeight: 200
    title: "Flowee Pay"

    onWidthChanged: Flowee.windowWidth = width
    onHeightChanged: Flowee.windowHeight = height
    onVisibleChanged: if (visible) ControlColors.applySkin(mainWindow)

    property bool isLoading: typeof wallets === "undefined";

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Quit")
                shortcut: StandardKey.Quit
                onTriggered: mainWindow.close()
            }
        }
        Menu {
            title: qsTr("&View")

            Action {
                checkable: true
                checked: Flowee.useDarkSkin
                text: qsTr("&Dark Mode")
                onTriggered: {
                    Flowee.useDarkSkin = checked
                    ControlColors.applySkin(mainWindow)
                }
            }
            Action {
                enabled: !mainWindow.isLoading
                text: qsTr("Network Status")
                onTriggered: {
                    netView.source = "NetView.qml"
                    netView.item.show();
                }
            }
            MenuSeparator { }
            Menu {
                title: qsTr("Unit Indicator")
                Action {
                    text: "mBCH"
                    checkable: true
                    checked: Flowee.unit === Pay.MilliBCH
                    onTriggered: Flowee.unit = Pay.MilliBCH
                }
                Action {
                    text: "BCH"
                    checkable: true
                    checked: Flowee.unit === Pay.BCH
                    onTriggered: Flowee.unit = Pay.BCH
                }
                Action {
                    text: "µBCH"
                    checkable: true
                    checked: Flowee.unit === Pay.MicroBCH
                    onTriggered: Flowee.unit = Pay.MicroBCH
                }
                Action {
                    text: "Bits"
                    checkable: true
                    checked: Flowee.unit === Pay.Bits
                    onTriggered: Flowee.unit = Pay.Bits
                }
                Action {
                    text: "Satoshis"
                    checkable: true
                    checked: Flowee.unit === Pay.Satoshis
                    onTriggered: Flowee.unit = Pay.Satoshis
                }
            }
        }
    }

    AccountSelectionPage {
        id: accountSelectionPage
        anchors.fill: parent
        visible: !isLoading && wallets.current === null
    }

    AccountPage {
        width: parent.width
    }

    // NetView (reachable from menu)
    Loader {
        id: netView
        onLoaded: {
            ControlColors.applySkin(item)
            netViewHandler.target = item
        }
        Connections {
            id: netViewHandler
            function onVisibleChanged() {
                if (!netView.item.visible)
                    netView.source = ""
            }
        }
    }

    footer: Pane {
        contentWidth: parent.width
        contentHeight: statusBar.height
        Label {
            id: statusBar
            property string message: ""
            text: {
                if (mainWindow.isLoading)
                    return qsTr("Starting up...");
                if (wallets.current === null)
                    return qsTr("%1 wallets").arg(wallets.accounts.length);
                if (message === "")
                    return wallets.current.name
                return message;
            }
        }

    }
}
