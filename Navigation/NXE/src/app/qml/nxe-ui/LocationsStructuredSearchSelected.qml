import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    width: 400
    height: 50
    color: "#09bcdf"

    signal carButtonClicked()
    property alias title: headerTextItem.text
    property alias description: descriptionItem.text

    RowLayout {
        anchors.fill: parent
        spacing: 5

        Item {
            id: locationInfoItem
            height: parent.height
            width: 300

            Layout.fillWidth: true

            Column {
                anchors.fill: parent
                anchors.leftMargin: 5
                Text {
                    id: headerTextItem
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 16
                    height: parent.height/2
                    width: parent.width
                }

                Text {
                    id: descriptionItem
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 16
                    height: parent.height/2
                    width: parent.width
                }
            }
        }

        Rectangle {
            id: separator
            width: 1
            height: parent.height
            color: "white"
        }

        Item {
            width: 50
            height: parent.height

            Image {
                anchors.centerIn: parent
                source: "car_icon_white.png"
                sourceSize.width: 40
                sourceSize.height: 40
                visible: root.height !== 0
            }

            MouseArea {
                anchors.fill: parent
                onClicked: carButtonClicked()
            }
        }
    }
}

