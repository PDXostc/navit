import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    width: 120
    height: 4 * (theme.menu.buttonSize + 30)
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: parent.width * 0.1

    signal clicked(string item)

    Rectangle {
        id: menuBackground
        color: theme.menu.backgroundColor
        anchors.fill: parent
        opacity: theme.menu.opacity
    }

    RectangularGlow {
        anchors.fill: menuBackground
        glowRadius: 8
        spread: 0.2
        color: "#66000000"
    }

    Column {
        anchors.fill: parent

        Item {
            width: parent.width
            height: root.height/4

            Image {
                id: menuImageItem
                anchors.centerIn: parent
                source: "menu_icon_white.png"
                sourceSize.width: theme.menu.buttonSize
                sourceSize.height: theme.menu.buttonSize
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
                sourceSize.width: theme.menu.buttonSize
                sourceSize.height: theme.menu.buttonSize
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
                source: navitProxy.orientation === 0 ? "northup_icon_white.png":"headup_icon_white.png"

                sourceSize.width: theme.menu.buttonSize
                sourceSize.height: theme.menu.buttonSize
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (navitProxy.orientation === 0) {
                        navitProxy.orientation = -1;
                    } else {
                        navitProxy.orientation = 0;
                    }
                }
            }
        }
        Item {
            width: parent.width
            height: root.height/4
            Image {
                anchors.centerIn: parent
                source: "car_icon_white.png"
                sourceSize.width: theme.menu.buttonSize
                sourceSize.height: theme.menu.buttonSize
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.clicked("currentLocation")
            }
        }
    }
}
