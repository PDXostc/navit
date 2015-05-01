import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {
    width: 400
    height: 800
    id: mainPageView

    NMenu {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: parent.width * 0.1
        onClicked: {
            if (item === "menu") {
                console.debug("menu clicked")
                rootStack.push(settingsView)
            } else if (item === "search") {
                rootStack.push(locationsView)
            } else if (item === "northOrientation") {
                console.debug("North orientation activated")
            } else if (item === "headOrientation") {
                console.debug("Head orientation activated")
            } else if (item === "currentLocation") {
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

    MapLocationInfo {
        visible: navitProxy.currentlySelectedItem !== null
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        locationName: navitProxy.currentlySelectedItem !== null ?
                          navitProxy.currentlySelectedItem.name : "";

        height: 200
    }

    Loader {
        id: topLocationBarLoader
        anchors.top: parent.top
    }
    Component {
        id: topLocationBar
        MapLocationInfoTop {
            width: mainPageView.width
        }
    }
    Component {
        id: locationsView
        LocationsView {
            onBackToMapRequest: rootStack.pop()
        }
    }
}
