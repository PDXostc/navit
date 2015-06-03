import QtQuick 2.0
import '../common'

Page {
    width: 400
    height: 800

    Rectangle {
        id: blue
        width: parent.width
        height: 450
        color: "#09bcdf"
        anchors.centerIn: parent

        Timer {
            running: true
            interval: 5000
            onTriggered: {

                if(navitProxy.ftu) {
                    navitProxy.ftu = false;
                } else {
                    navitProxy.restartNavit();
                }
                rootStack.clear();
                rootStack.push({
                                   item: Qt.resolvedUrl("../MainPage.qml")
                               });
            }
        }

        Image {
            id: logo
            width: 96
            height: 96
            anchors.top: parent.top
            anchors.topMargin: -width/2
            anchors.horizontalCenter: parent.horizontalCenter
            source: "../images/download_hex_icon.png"
        }

        Item {
            width: parent.width
            height: parent.height/2
            anchors.centerIn: parent

            NText {
                anchors.fill: parent
                text: "Downloads completed"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 48
                wrapMode: Text.WordWrap
            }
        }
    }
}
