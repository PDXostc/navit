import QtQuick 2.0
import QtQuick.Controls 1.3

Page {

    Item {
        anchors.top: parent.top
        width: parent.width
        height: 60

        Row {
            anchors.fill: parent
            spacing: 10

            Image {
                source: "search_icon_white_sm.png"
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - 10
                font.pixelSize: 20
            }
        }
    }
}
