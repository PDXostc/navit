import QtQuick 2.0

Item {
    id: locationInfo
    x: 0
    y: 0
    width: 400
    height: 50
    visible: true
    signal backToMapRequest()
    property bool isFavorite : false
    property bool canNavigate : false
    property bool showInfo : false
    Item {
        id: item3
        anchors.fill: parent
        Item {
            id: buttonsBar
            x: 0
            y: 0
            width: 60
            height: parent.height

            Rectangle {
                id: rectangle2
                color: "#000000"
                opacity: 0.85
                anchors.fill: parent


                MouseArea {
                    id: topLocationBarBackButton
                    anchors.fill: parent
                    onClicked: showInfo = true
                }

                Image {
                    id: info
                    x: 21
                    y: 0
                    width: 17
                    height: 40
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "info_icon_white.png"
                }
            }
        }

        Item {
            visible: showInfo
            id: item1
            x: 58
            y: 0
            anchors.left: buttonsBar.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.leftMargin: 1

            Rectangle {
                id: rectangle1
                color: "#000000"
                opacity: 0.85
                anchors.fill: parent

                Text {
                    id: text1
                    x: 8
                    y: 8
                    color: "#ffffff"
                    text: qsTr("Current:")
                    font.pixelSize: 12
                }

                Text {
                    id: text2
                    x: 8
                    y: 29
                    color: "#ffffff"
                    text: qsTr("Next:")
                    font.pixelSize: 12
                }

                Text {
                    id: text3
                    x: 79
                    y: 8
                    color: "#ffffff"
                    text: qsTr("14th")
                    font.pixelSize: 12
                }

                Text {
                    id: text4
                    x: 79
                    y: 27
                    color: "#ffffff"
                    text: qsTr("Overton (:30sec/0.1 miles)")
                    font.pixelSize: 12
                }

            }

            MouseArea {
                id: mouseArea1
                x: 312
                width: 20
                height: 20
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                onClicked: showInfo = false
                Text {
                    id: text5
                    color: "#ffffff"
                    text: qsTr("x")
                    font.bold: false
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 13
                }
            }
        }
    }
}
