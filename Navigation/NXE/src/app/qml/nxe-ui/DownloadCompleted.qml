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

        Timer {
            running: true
            interval: 5000
            onTriggered: {

                navitProxy.ftu = false
                rootStack.clear()
                rootStack.push({
                                   item: Qt.resolvedUrl("MainPage.qml")
                               })
            }
        }

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

        Item {
            width: 200
            height: 122
            anchors.centerIn: parent

            Text {
                anchors.fill: parent
                text: "Downloads completed"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 23
                wrapMode: Text.WordWrap
                color: "white"
            }
        }
    }
}
