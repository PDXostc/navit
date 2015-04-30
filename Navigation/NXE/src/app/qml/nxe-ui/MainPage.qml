import QtQuick 2.0
import QtQuick.Layouts 1.1

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
            } else if (item === "search") {
                if (bottomLocationBarLoader.sourceComponent !== null) {
                    topLocationBarLoader.sourceComponent = null
                    bottomLocationBarLoader.sourceComponent = null
                } else {
                    topLocationBarLoader.sourceComponent = topLocationBar
                    bottomLocationBarLoader.sourceComponent = bottomLocationBar
                }
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
    Loader {
        id: bottomLocationBarLoader
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
    }
    Component {
        id: bottomLocationBar
        MapLocationInfo {
            width: mainPageView.width
        }
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
