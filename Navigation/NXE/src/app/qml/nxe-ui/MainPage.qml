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

    function finishComponentCreation(location) {
        if (locationInfoComponent.status === Component.Ready) {
            locationInfoObject = locationInfoComponent.createObject(
                        mainPageView, {
                            opacity: 0
                        })
            locationInfoObject.anchors.bottom = mainPageView.bottom
            locationInfoObject.anchors.left = mainPageView.left
            locationInfoObject.anchors.right = mainPageView.right
            locationInfoObject.locationComponent = location
            locationInfoObject.opacity = 1
        }
    }
    function finishTopComponentCreation() {
        if (locationInfoTopComponent.status === Component.Ready) {
            locationInfoTopObject = locationInfoTopComponent.createObject(
                        mainPageView, {
                            opacity: 0
                        })
            locationInfoTopObject.anchors.top = mainPageView.top
            locationInfoTopObject.anchors.left = mainPageView.left
            locationInfoTopObject.anchors.right = mainPageView.right
            locationInfoTopObject.locationComponent = navitProxy.currentlySelectedItem
            locationInfoTopObject.opacity = 1

            locationInfoTopObject.requestHideBars.connect(function() {
                removeInfo();
                removeTopInfo()
            })
        }
    }

    function createLocationComponent(location) {
        locationInfoComponent = Qt.createComponent("MapLocationInfo.qml")
        locationInfoTopComponent = Qt.createComponent("MapLocationInfoTop.qml")
        if (locationInfoComponent.status === Component.Ready) {
            finishComponentCreation(location)
        } else {
            locationInfoComponent.statusChanged.connect(finishComponentCreation(
                                                            location))
        }
        if (locationInfoTopComponent.status === Component.Ready) {
            finishTopComponentCreation(location)
        } else {
            locationInfoTopComponent.statusChanged.connect(
                        finishTopComponentCreation(location))
        }
    }

    function removeInfo() {

        if (locationInfoComponent) {
            locationInfoComponent.destroy()
            locationInfoObject.destroy()
        }
    }

    function removeTopInfo() {

        if (locationInfoTopComponent) {
            locationInfoTopComponent.destroy()
            locationInfoTopObject.destroy()
        }
    }

    NMenu {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 110

        onClicked: {
            if (item === "menu") {
                rootStack.push(settingsView)
            } else if (item === "search") {
                rootStack.push(locationsView)
            } else if (item === "currentLocation") {
                navitProxy.moveToCurrentPosition()
            }
        }
    }

    Connections {
        target: navitProxy
        onCurrentlySelectedItemChanged: {
            removeInfo()
            removeTopInfo()

            if (navitProxy.currentlySelectedItem) {
                createLocationComponent(navitProxy.currentlySelectedItem)
            }
        }

        onPointClicked: {
            if (locationInfoComponent && locationInfoObject) {
                locationInfoObject.locationComponent = location
            } else {
                console.debug(location.itemText)
                createLocationComponent(location)
            }
        }

        onNavigationStarted: {
            removeInfo()
            locationInfoTopObject.extraInfoVisible =true;
        }

        onNavigationStopped: {
            console.debug('canceled')
            locationInfoTopObject.extraInfoVisible = false;
            removeInfo()
            removeTopInfo()

            // destroy everything!
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
