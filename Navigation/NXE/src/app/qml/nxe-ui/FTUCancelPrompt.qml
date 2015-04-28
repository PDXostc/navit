import QtQuick 2.0

Rectangle {
    id: rectangle1
    width: 400
    height: 800
    color: "black"

    Rectangle {
        id: blue
        width: parent.width
        height: 300
        color: "#09bcdf"
        anchors.centerIn: parent

        Item {
            width: 200
            height: 122
            anchors.centerIn: parent

            Text {
                anchors.fill: parent
                text: "Navit can't run without a map"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 23
                wrapMode: Text.WordWrap
                color: "white"
            }
        }


        Item {
            x: 195
            y: 230
            width: 157
            height: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 48

            Item {
                width: 162
                anchors.fill: parent

                Text {
                    id: text
                    text: "Download a map"
                    font.pointSize: 17
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white"
                }

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: text.right
                    anchors.leftMargin: 5
                    source: "next_icon_white.png"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {rootStack.push(Qt.resolvedUrl("CountryLanguage.qml"))}
                }
            }
        }

        Item {
            id: item1
            x: 33
            y: 240
            width: 112
            height: 40

            Text {
                id: text1
                y: 3
                color: "#ffffff"
                text: qsTr("Quit")
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 17
            }

            MouseArea {
                id: cancelMouseArea
                width: 112
                height: 40
                onClicked: {Qt.quit()}
            }
        }
    }

    Image {
        id: logo
        y: 216
        width: 64
        height: 64
        anchors.horizontalCenter: parent.horizontalCenter
        source: "alert_hex_icon.png"
    }
    Text {
            anchors.top: logo.bottom
            text: "Are you sure?"
            font.bold: true
            font.pointSize: 22
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
     }

}


