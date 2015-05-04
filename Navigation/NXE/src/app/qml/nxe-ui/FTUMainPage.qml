import QtQuick 2.0

Page {
    width: 400
    height: 800

    Rectangle {
        id: blue
        width: parent.width
        height: 300
        color: "#09bcdf"
        anchors.centerIn: parent

        Image {
            id: logo
            x: 168
            width: 64
            height: 64
            anchors.top: parent.top
            anchors.topMargin: -32
            anchors.horizontalCenter: parent.horizontalCenter
            source: "download_hex_icon.png"
        }

        Text {
            text: "Welcome"
            font.pixelSize: 29
            font.bold: true
            anchors.top: logo.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
        }

        Item {
            width: 200
            height: 122
            anchors.centerIn: parent

            Text {
                anchors.fill: parent
                text: "Before you can use NavIT, you need to download a map."
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 23
                wrapMode: Text.WordWrap
                color: "white"
            }
        }

        Item {
            width: 200
            height: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            Item {
                width: 172
                anchors.fill: parent

                Text {
                    id: text
                    text: "Download a map"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 24
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white"
                }

                Image {
                    height: 32
                    anchors.left: text.right
                    anchors.leftMargin: 5
                    source: "next_icon_white.png"
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        rootStack.push({item:Qt.resolvedUrl("SettingsView.qml"), properties: {ftu: true}})
                    }
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
                text: qsTr("Cancel")
                font.pixelSize: 24
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            MouseArea {
                id: cancelMouseArea
                width: 112
                height: 40
                onClicked: {
                    rootStack.push(Qt.resolvedUrl("FTUCancelPrompt.qml"))
                }
            }
        }

    }
}
