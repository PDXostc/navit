import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: root
    width: 400
    height: 800
    color: "black"

    signal backToMapRequest
    property bool ftu: false

    Column {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 10

        spacing: 10

        Item {
            id: header
            width: root.width - 20
            height: 47

            Row {
                anchors.fill: parent
                anchors.bottomMargin: 10
                spacing: 0

                MenuHeaderButton {
                    id: backButton
                    width: 30
                    height: parent.height
                    iconSource: "back_icon_white_lg.png"
                    iconWidth: 24
                    iconHeight: 24

                    onClicked: {
                        if (settingsStackView.depth !== 1) {
                            settingsStackView.pop()
                        } else {
                            backToMapRequest()
                        }
                    }
                }

                Text {
                    width: parent.width - 40 - 60
                    height: parent.height
                    text: "Settings"
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 24
                }

                Item {
                    width: 70
                    height: parent.height

                    MenuHeaderButton {
                        id: bckB
                        width: 16
                        height: 35
                        iconSource: "back_icon_white_sm.png"
                        onClicked: backToMapRequest()
                    }

                    MenuHeaderButton {
                        id: mapBackButton
                        width: 34
                        height: 33
                        iconWidth: 24
                        iconHeight: 24
                        iconSource: "map_icon_white.png"
                        anchors.left: bckB.right
                        anchors.leftMargin: 5
                        onClicked: backToMapRequest()
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 2

                anchors.bottom: parent.bottom
            }
        }

        StackView {
            id: settingsStackView
            Component.onCompleted: {
                if (ftu) {
                    stack.push(Qt.resolvedUrl("CountryLanguage.qml"))
                    backButton.enabled = false
                    mapBackButton.enabled = false
                    header.opacity = 0.4
                } else {
                }
            }

            initialItem: Item {
                width: parent.width
                height: parent.height - 100
                SettingsListView {
                    id: settingsListView
                    model: SettingsListModel {
                    }
                    anchors.fill: parent
                    clip: true
                    onSubMenuRequest: settingsStackView.push(
                                          Qt.resolvedUrl(url))
                }
                ScrollBar {
                    flk: settingsListView
                }
            }
        }
    }
}
