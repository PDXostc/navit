import QtQuick 2.0
import QtQuick.Layouts 1.1

Item {
    id: root
    width: 400
    height: 800

    property alias dialogText: mainTextItem.text
    property alias headerText: headerTextItem.text
    property alias iconSource: logoImage.source
    property Component bottomBarComponent: null
    property alias bckColor: blueRectangle.color

    Rectangle {
        id: blueRectangle
        width: parent.width
        height: 450
        color: "#09bcdf"
        anchors.centerIn: parent
    }

    Column {
        width: parent.width
        height: blueRectangle.height + theme.dialog.buttonSize/2
        anchors.top: blueRectangle.top
        anchors.topMargin: -theme.dialog.buttonSize/2
        spacing: 5

        Image {
            id: logoImage
            width: theme.dialog.buttonSize
            height: theme.dialog.buttonSize
            anchors.horizontalCenter: parent.horizontalCenter
            source: "download_hex_icon.png"
        }

        NText {
            id: headerTextItem
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: theme.bigFontSize
        }

        Item {
            id: spacer
            height: 20
            width: parent.width
        }

        Item {
            width: parent.width
            height: 200

            NText {
                id: mainTextItem
                anchors.margins: 10
                width: parent.width / 2
                anchors.centerIn: parent

                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }

        Item {
            id: buttonBar
            width: parent.width
            height: theme.dialog.bottomBarHeight

            Loader {
                sourceComponent: root.bottomBarComponent
                anchors.fill: parent
            }
        }
    }
}


