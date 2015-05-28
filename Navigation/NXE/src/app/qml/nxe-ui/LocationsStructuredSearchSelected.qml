import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    width: 400
    height: 50
    color: theme.ssearchView.selectedBck

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
                NText {
                    id: headerTextItem
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height/2
                    width: parent.width
                }

                NText {
                    id: descriptionItem
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height/2
                    width: parent.width
                    font.pixelSize: theme.defFontSize - 6
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
            width: theme.defButtonSize + 12
            height: parent.height

            Image {
                anchors.centerIn: parent
                source: "car_icon_white.png"
                sourceSize.width: theme.defButtonSize
                sourceSize.height: theme.defButtonSize
                visible: root.height !== 0
            }

            MouseArea {
                anchors.fill: parent
                onClicked: carButtonClicked()
            }
        }
    }
}

