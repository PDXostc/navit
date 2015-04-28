import QtQuick 2.0


// Blueish dialog
Item {
    id: dialog
    property int numberOfDowloads: 0
    property real downloadSize: 0

    signal startDownloadClicked()

    Rectangle {
        id: rectangle1
        width: parent.width
        height: 167
        color: "#09bcdf"
        anchors.bottom: parent.bottom

        Text {
            color: "white"
            text: "(" + count + ") Downloads (" + downloadSize + "MB)"
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60
            font.pixelSize: 16
        }

        MouseArea {
            width: 70
            height: 50
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 48

            onClicked: startDownloadClicked()

            Row {
                id: row1
                width: 83
                anchors.fill: parent
                Text {
                    width: 75
                    color: "white"
                    text: "Start Download"
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                }

                Image {
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
