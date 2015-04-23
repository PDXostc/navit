import QtQuick 2.0

Item {
    width: 400
    height: 800

    signal pushToStack(var component)

    NMenu {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: parent.width * 0.1

        onClicked: {
            if (item === "menu") {
                pushToStack(settingsView)
            } else if(item === 'orientation') {
                navitProxy.orientation = -1;
            } else if(item === "search") {
                pushToStack(searchPageView)
            }
        }
    }

    Rectangle {
        width: 300
        height: 100
        opacity: 0.7
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color: "black"

        Text {
            anchors.centerIn: parent
            text: navitProxy.version
            font.pixelSize: 18
            color: "white"
        }
    }

    Rectangle {
        width: 300
        height: 100
        opacity: 0.7
        anchors.bottom: parent.bottom
        anchors.left:parent.left
        color: "black"

        Text {
            anchors.centerIn: parent
            text: navitProxy.position
            font.pixelSize: 18
            color: "white"
        }
    }

    Component {
        id: settingsView

        SettingsView {
            onBackToMapRequest: rootStack.pop()
        }
    }

    Component {
        id: searchPageView
        SearchPage {
        }
    }
}
