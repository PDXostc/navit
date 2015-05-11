import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: header
    width: 400
    height: 50
    property bool isHeaderEnabled: true
    opacity: isHeaderEnabled ? 1 : 0.4

    property alias header: backButton.text
    property var stack: null

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    Row {
        anchors.fill: parent
        spacing: 0

        NButton {
            id: backButton
            height: parent.height
            width: 100
            iconSource: "back_icon_white_lg.png"
            iconWidth: 24
            iconHeight: 24
            layout: Qt.RightToLeft
            enabled: isHeaderEnabled

            onClicked: {
                if (stack.depth !== 1) {
                    stack.pop()
                } else {
                    backToMapRequest()
                }
            }
        }

        Item {
            width: parent.width - backButton.width - backButtonItem.width
            height: parent.height
            Layout.fillWidth: true
        }

        Item {
            id: backButtonItem
            width: 50
            height: parent.height

            NButton {
                id: bckB
                width: 16
                height: parent.height
                iconSource: "back_icon_white_sm.png"
                onClicked: backToMapRequest()
                enabled: isHeaderEnabled
            }

            NButton {
                id: mapBackButton
                width: 24
                height: parent.height
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
        height: 1
        anchors.bottom: parent.bottom
        color: "white"
    }
}
