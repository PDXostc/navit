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
            }
        }
    }

    Component {
        id: settingsView

        SettingsView {
            onBackToMapRequest: rootStack.pop()
        }
    }
}
