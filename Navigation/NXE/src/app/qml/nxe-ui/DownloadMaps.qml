import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Page {

    property var maps

    Component.onCompleted: {
        console.debug(maps)
    }

    Rectangle {
        id: rect
        width: parent.width
        height: 300
        color: "#09bcdf"
        anchors.verticalCenter: parent.verticalCenter
    }

    Image {
        id: icon
        anchors.bottomMargin: -31
        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.height: 64
        sourceSize.width: 64
        source: "download_hex_icon.png"
        anchors.bottom: rect.top
    }

    Column {
        spacing: 10
        anchors {
            top: icon.bottom
            topMargin: 10
            bottom: rect.bottom
            left: rect.left
            leftMargin: 10
            right: rect.right
            rightMargin: 10
        }
        Text {
            id: headerText
            color: "white"
            text: "Downloading (1 of 3)"
            font.pointSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ProgressBar {
            width: parent.width
            height: 15
            value: 0.4
            style: ProgressBarStyle {
                background: Rectangle {
                    radius: 2
                    color: "#467C8A"
                    border.color: "#467C8A"
                    border.width: 1
                    implicitWidth: 200
                    implicitHeight: 8
                }
                progress: Rectangle {
                    color: "#BABABA"
                    border.color: "#BABABA"
                }
            }
        }
        Item {
            width: parent.width
            height: 20
            Text {
                text: maps[0]
                font.pointSize: 16
                color: "white"
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "10.9MB/75MB"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
        }

        Item {
            width: parent.width
            height: 20
            Text {
                text: "Time remaining"
                font.pointSize: 16
                color: "white"
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "1.3 min"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
        }
    }

    Item {
        id: item1
        width: parent.width
        height: 40
        anchors.bottom: rect.bottom

        Button {
            text: "Cancel"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            iconSource: "next_icon_white.png"
            LayoutMirroring.enabled: true

            onClicked: {
                rootStack.pop();
            }
        }
    }
}
