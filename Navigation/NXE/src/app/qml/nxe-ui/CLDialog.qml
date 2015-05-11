import QtQuick 2.0

// Blueish dialog
Item {
    id: dialog
    property int numberOfDowloads: 0
    property real downloadSize: 0
    width: 400
    height: 200

    signal startDownloadClicked()

    Rectangle {
        width: parent.width
        height: parent.height - 32
        color: "#09bcdf"
        anchors.bottom: parent.bottom

        Text {
            color: "white"
            text: "(" + count + ") Downloads (" + downloadSize + "MB)"
            anchors.left: parent.left
            anchors.leftMargin: 10
            font.pixelSize: 16
            anchors.verticalCenter: parent.verticalCenter
        }

        MouseArea {
            width: 70
            height: 50
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.verticalCenter: parent.verticalCenter

            onClicked: startDownloadClicked()

            Row {
                width: 93
                anchors.fill: parent
                spacing: 5

                Text {
                    width: 75
                    color: "white"
                    text: "Start Download"
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                }

                Image {
                    width: 13
                    anchors.verticalCenter: parent.verticalCenter
                    sourceSize.height: 20
                    sourceSize.width: 20
                    source: "next_icon_white.png"
                }
            }
        }
    }
    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.height: 64
        sourceSize.width: 64
        source: "download_hex_icon.png"
    }
}
