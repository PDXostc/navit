import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {
    id: mainPageView
    width: 400
    height: 800

    property var locationInfoComponent: null
    property var locationInfoTopComponent: null
    property var locationInfoObject: null
    property var locationInfoTopObject: null
    property bool topBarExpanded : false

    function finishComponentCreation(location) {
        if (locationInfoComponent.status === Component.Ready) {
            locationInfoObject = locationInfoComponent.createObject(mainPageView);
            locationInfoObject.anchors.bottom = mainPageView.bottom
            locationInfoObject.anchors.left = mainPageView.left
            locationInfoObject.anchors.right = mainPageView.right
            locationInfoObject.locationComponent = location
        }
    }
    function finishTopComponentCreation() {
        if (locationInfoTopComponent.status === Component.Ready) {
            locationInfoTopObject = locationInfoTopComponent.createObject(mainPageView);
            locationInfoTopObject.anchors.top = mainPageView.top
            locationInfoTopObject.anchors.left = mainPageView.left
            locationInfoTopObject.anchors.right = mainPageView.right
            locationInfoTopObject.locationComponent = navitProxy.currentlySelectedItem;
        }
    }

    function createLocationComponent(location) {
        locationInfoComponent = Qt.createComponent("MapLocationInfo.qml");
        locationInfoTopComponent = Qt.createComponent("MapLocationInfoTop.qml");
        if (locationInfoComponent.status === Component.Ready) {
            finishComponentCreation(location);
        } else {
            locationInfoComponent.statusChanged.connect(finishComponentCreation(location));
        }
        if (locationInfoTopComponent.status === Component.Ready) {
            finishTopComponentCreation(location);
        } else {
            locationInfoTopComponent.statusChanged.connect(finishTopComponentCreation(location));
        }
    }

    NMenu {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: topBarExpanded ? 110 : 70 // parent.width * 0.2 : 70

        onClicked: {
            if (item === "menu") {
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

    Connections {
        target: navitProxy
        onCurrentlySelectedItemChanged:{
        if(navitProxy.currentlySelectedItem === null) {
            locationInfoComponent.destroy();
            locationInfoTopComponent.destroy();
            locationInfoTopObject.destroy();
            locationInfoObject.destroy();
        } else
            createLocationComponent(navitProxy.currentlySelectedItem);
        }

        onPointClicked: {
            if (locationInfoComponent && locationInfoObject) {
                locationInfoObject.locationComponent = location;
            } else {
                console.debug(location.itemText)
                createLocationComponent(location)
            }
        }
        onTopBarLocationVisibleChanged: {
                   mainPageView.topBarExpanded = navitProxy.topBarLocationVisible;
        }

    }

    Component {
        id: settingsView
        SettingsView {
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
