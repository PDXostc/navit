import QtQuick 2.0
import QtQml 2.2
import '../common'

Page {
    id: root

    Rectangle {
        width: 380
        height: 140
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10

        TextEdit {
            id: searchInput
            color: "#242424"
            anchors {
                fill: parent
                horizontalCenter: parent.horizontalCenter
                left: parent.left
                leftMargin: 6
                top: parent.top
                topMargin: 6
            }
            font.pixelSize: 11
            text: "Start searching..."
            onFocusChanged: text = ""

            MouseArea {
                id: clearInput
                onClicked: searchInput.text = ""
                width: 30
                height: 30
                anchors {
                    bottom: parent.bottom
                    bottomMargin: 5
                    right: parent.right
                    rightMargin: 5
                }
                Text {
                    text: qsTr("x")
                    font.pixelSize: 16
                    anchors.centerIn: parent
                }
            }
        }
    }

    Item {
        id: item1
        x: 96
        y: 162
        width: 150
        height: 42
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter

        Image {
            id: image1
            width: 150
            height: 42
            source: "search_button_bg.png"
        }

        Image {
            id: image2
            width: 16
            height: 17
            anchors.verticalCenterOffset: 0
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            source: "search_icon_white_sm.png"
        }

        Text {
            id: text2
            x: 32
            y: 13
            color: "#ffffff"
            text: qsTr("Search")
            font.pixelSize: 10
        }
        MouseArea {
            id: performSearch
            width: 142
            height: 42
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                // for some reasons weekeyboard needs to be manually hidden
                Qt.inputMethod.hide()
                searchInput.focus = false
                root.busy = true
                navitProxy.searchNear(searchInput.text)
            }
        }
    }

    Connections {
        target: navitProxy
        // @disable-check M16
        onSearchDone: {
            root.busy = false
        }
    }
}
