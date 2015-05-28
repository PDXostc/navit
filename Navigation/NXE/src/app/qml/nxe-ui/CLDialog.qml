import QtQuick 2.0

// Blueish dialog
Item {
    id: dialog
    property int numberOfDowloads: 0
    property real downloadSize: 0
    width: 800
    height: 400

    signal startDownloadClicked()

    Rectangle {
        width: parent.width
        height: parent.height - 96/2
        color: "#09bcdf"
        anchors.bottom: parent.bottom

        NText {
            text: "(" + count + ") Downloads (" + downloadSize + "MB)"
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        NButton {
            text: "Start Download"
            iconType: "arrow_right"
            onClicked: startDownloadClicked()
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.height: 96
        sourceSize.width: 96
        source: "download_hex_icon.png"
    }
}
