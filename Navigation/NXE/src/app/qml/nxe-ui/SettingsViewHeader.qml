import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQml 2.2

Item {
    id: header
    width: 400
    height: 50
    property bool isHeaderEnabled: true
    opacity: isHeaderEnabled ? 1 : 0.4

    property alias header: backButton.text
    property string subHeader
    property var stack: null

    onSubHeaderChanged: {
        if (subHeader === "") {
            subHeaderText.text = ""
        } else {
            subHeaderText.text = " /" + subHeader
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    Row {
        anchors.fill: parent
        spacing: 0
        Item {
            width: 15
            height: parent.height
        }

        NButton {
            id: backButton
            height: parent.height
            width: 150
            iconType: "arrow_left"
            iconHeight: 36
            layout: Qt.RightToLeft
            enabled: isHeaderEnabled
            onClicked: {
                Qt.inputMethod.hide()
                if (stack.depth !== 1) {
                    stack.pop()
                    subHeaderText.text = ""
                } else {
                    backToMapRequest()
                }
            }
        }

        Item {
            width: parent.width - backButton.width - backButtonItem.width - 15
            height: parent.height
            Layout.fillWidth: true

            NText {
                id: subHeaderText
                anchors.leftMargin: 5
                color: "gray"
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 1
                font.pixelSize: 14
            }
        }

        Row {
            id: backButtonItem
            width: 50
            height: parent.height

            NButton {
                id: bckB
                width: 16
                height: parent.height
                iconType: "arrow_left"
                iconWidth: 16
                iconHeight: 36
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
                onClicked: {
                    Qt.inputMethod.hide()
                    backToMapRequest()
                }
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
