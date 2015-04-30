import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: locationsListRoot
    width: 400
    height: 800
    color: "black"

    signal backToMapRequest
    signal showLocationRequest
    property string headerSmallText
    property var choosenLocation

    onHeaderSmallTextChanged: {
        if(headerSmallText !== "") {
            slashSymbol.text = "/";
            smallText.text = headerSmallText
        }
        else {
            slashSymbol.text = "";
            smallText.text = "";
        }
    }
    Column {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 10

        spacing: 10

        Item {
            id: header
            width: locationsListRoot.width - 20
            height: 47

            Row {
                anchors.fill: parent
                anchors.bottomMargin: 10
                spacing: 0

                MenuHeaderButton {
                    width: 30
                    height: parent.height
                    iconSource: "back_icon_white_lg.png"
                    iconWidth: 24
                    iconHeight: 24

                    onClicked: {
                        if (stack.depth !== 1) {
                            if(stack.depth === 2) {
                                locationsListRoot.headerSmallText = ""
                            }
                            stack.pop()
                        } else {
                            backToMapRequest()
                        }
                    }
                }

                Text {
                    width: parent.width - 40 - 60
                    height: parent.height
                    text: "Locations"
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

            Text {
                id: slashSymbol
                x: 135
                y: 2
                width: 11
                height: 29
                color: "#696969"

                font.pointSize: 19
            }

            Text {
                id: smallText
                x: 150
                y: 11
                width: 64
                height: 20
                color: "#ffffff"
                font.pointSize: 13
            }
        }

        StackView {
            id: stack
            initialItem: LocationsListView {
                id: locationsListView
                model: LocationsListModel {}
                width: parent.width
                height: parent.height - 100
                clip: true
                onSubMenuRequest: stack.push(Qt.resolvedUrl(url))
            }
        }
    }
}
