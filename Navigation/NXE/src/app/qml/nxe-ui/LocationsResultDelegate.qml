import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: delegateroot
    width: LocationResultListView.width
    height: 50

    MouseArea {
        anchors.fill: parent

        onClicked: {
            mainPageView.pushToStack(mapLocationInfoBar)
        }
    }
    Row {
        id: row1
        anchors.fill: parent
        Item {
            id: item1
            width: parent.width
            height: parent.height
            Text {
                id: mainTextItem
                width: parent.width * 2 / 3
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: itemText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: starImage.right
                anchors.leftMargin: 5
                color: "white"
                font.pixelSize: 18
            }

            Image {
                id: starImage
                width: favorite === true ? 20 : 0
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                source: "star_icon_solid_white.png"
            }

            Text {
                id: descriptionText
                color: "#ffffff"
                text: description !== undefined ? description : ""
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 5
                font.pixelSize: 12
                font.bold: bolded
            }
        }
    }
    Rectangle {
        width: parent.width
        height: 1
        color: "#45a6c3"
        anchors.bottom: parent.bottom
    }

}
