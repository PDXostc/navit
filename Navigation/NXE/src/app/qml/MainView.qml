import QtQuick 2.0
import QtQuick.Controls 1.0

Item {
    id: root
    width: 1080
    height: 1600

    StackView {
        id: stack
        anchors.fill: parent

        initialItem: Item {
            width: root.width
            height: root.height
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
    }

    Item {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 100

        Rectangle {
            anchors.fill: parent
            color: "transparent"
        }

        Row {
            anchors.fill: parent

            Button {
                id: zoomIn
                width: 100
                height: 100
                text: "zoom in"

                onClicked: {
                    navitProxy.zoomIn()
                }
            }

            Button {
                id: zoomOut
                width: 100
                height: 100
                text: "zoom out"

                onClicked: {
                    navitProxy.zoomOut()
                }
            }

            Button {
                id: showMenu
                width: 100
                height: 100
                text: "show menu"

                onClicked: {
                    stack.push("qrc:/qml/Menu.qml");
                }
            }
        }
    }
}
