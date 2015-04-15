import QtQuick 2.0
import Navit 1.0

import QtQuick.Controls 1.2

Item {
    width: 1080
    height: 1600

//    NavitItem {
//        id: navitItem
//        anchors.fill: parent

//        MouseArea {
//            anchors.fill: parent
//            property var oldMouseX
//            property var oldMouseY

//            // TODO: Implement proper zoom in (two click)
//            // zoom out (triple click) and drag
//            onDoubleClicked: {
//            }

//            onClicked: {
//                navitProxy.moveTo(mouse.x, mouse.y)
//            }

//            onReleased: {
//                var newMouse = mouse
//                console.debug(newMouse.x, newMouse.y);
//                var deltaX = oldMouseX - newMouse.x
//            }

//            onPressed: {
//                oldMouseX = mouse.x
//                oldMouseY = mouse.y
//            }
//        }
//    }

    Item {
        id: leftMenu
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 100

        width: 60
        height: 300

        Column {
            anchors.fill: parent

            Button {
                id: menu
            }

            Button {
                id: search
            }

            Button {
                id: orientation
                text: "orientation " + navitProxy.orientation
                onClicked: {
                    if (navitProxy.orientation === 0) {
                        navitProxy.orientation = -1;
                    } else {
                        navitProxy.orientation = 0
                    }
                }
            }

            Button {
                id: car
            }
        }
    }

    Item {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 120

        Row {
            anchors.fill: parent
            spacing: 10
            Button {
                width: 60
                height: 60
                text: "zoom in"

                onClicked: {
                    navitProxy.zoomIn();
                }
            }

            Button {
                width: 60
                height: 60
                text: "zoom out"

                onClicked: {
                    navitProxy.zoomOut();
                }
            }
        }
    }
}


