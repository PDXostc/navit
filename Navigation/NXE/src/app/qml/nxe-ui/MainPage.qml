import QtQuick 2.0

Item {
    width: 400
    height: 800
    id: mainPageView
    signal pushToStack(var component)

    NMenu {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: parent.width * 0.1
        onClicked: {
            if (item === "menu") {
                pushToStack(settingsView)
            }
            else if (item === "search")
            {
                pushToStack(locationsView)
            }
            else if (item === "northOrientation") {
                console.debug("North orientation activated")
            }
            else if (item === "headOrientation") {
                console.debug("Head orientation activated")
            }
            else if (item === "currentLocation") {
                console.debug("Current location button clicked")
            }
        }
    }
    Component {
        id: settingsView
        SettingsView {
            onBackToMapRequest: rootStack.pop()
        }
    }
    Component {
        id: mapLocationInfoBar
        MapLocationInfo {
            onBackToMapRequest: rootStack.pop()
       }
    }
    Component {
        id: locationsView
        LocationsView {
            onBackToMapRequest: rootStack.pop()

        }
    }

}

