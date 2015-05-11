import QtQuick 2.0

Item {
    id: root
    width: 60
    height: 4 * 60
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: parent.width * 0.1

    signal clicked(string item)

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.85
    }

    Column {
        anchors.fill: parent

        Item {
            width: parent.width
            height: root.height/4

            Image {
                anchors.centerIn: parent
                source: "menu_icon_white.png"
                sourceSize.width: 24
                sourceSize.height: 24
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.clicked('menu')
            }
        }

        Item {
            width: parent.width
            height: root.height/4
            Image {
                anchors.centerIn: parent
                source: "search_icon_white_sm.png"
                sourceSize.width: 24
                sourceSize.height: 24
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.clicked('search')
            }
        }
        Item {
            width: parent.width
            height: root.height/4
            Image {
                anchors.centerIn: parent
                source: navitProxy.orientation === -1 ? "northup_icon_white.png":"headup_icon_white.png"

                sourceSize.width: 24
                sourceSize.height: 24
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.clicked("headOrientation")
            }

        }
        Item {
            width: parent.width
            height: root.height/4
            Image {
                anchors.centerIn: parent
                source: "car_icon_white.png"
                sourceSize.width: 24
                sourceSize.height: 24
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.clicked("currentLocation")
            }
        }
    }
}
