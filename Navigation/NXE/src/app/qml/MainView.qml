import QtQuick 2.0
import Navit 1.0

Rectangle {
    width: 1080
    height: 1600
    color: "white"

    NavitItem {
        id: navitItem
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent

            // TODO: Implement proper zoom in (two click)
            // zoom out (triple click) and drag
            onDoubleClicked: {
//                console.debug('dc')
                navitProxy.zoomIn();
            }

            onClicked: {
//                console.debug('clicked');
                navitProxy.zoomOut();
            }

            onReleased: {
                console.debug('Released');
            }
        }
    }

    Item {
        id: leftMenu
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 100

        width: 60
        height: 300

        Rectangle {
            anchors.fill: parent
            color: "black"
        }
    }
}

