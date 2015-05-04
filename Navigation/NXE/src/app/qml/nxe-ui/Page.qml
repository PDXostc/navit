import QtQuick 2.0
import QtQuick.Controls 1.3

Rectangle {
    id: root
    width: 400
    height: 800

    property bool busy: false

    onBusyChanged: {
        if (busy) {
            root.state = "busy"
        } else {
            root.state = "normal"
        }
    }

    color: "black"
    state: "normal"

    Rectangle {
        id: busyRect
        anchors.fill: parent
        color: "black"
        z: 100

        Behavior on opacity { NumberAnimation {} }

        BusyIndicator {
            anchors.centerIn: parent
            running: root.state == "busy"
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: root
                enabled: true
            }
            PropertyChanges {
                target: busyRect
                opacity: 0
            }
        },
        State {
            name: "busy"
            PropertyChanges {
                target: root
                enabled: false
            }
            PropertyChanges {
                target: busyRect
                opacity: 0.7
            }
        }
    ]
}

