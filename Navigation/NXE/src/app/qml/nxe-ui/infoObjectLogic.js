function finishComponentCreation(location) {
    if (locationInfoComponent.status === Component.Ready) {
        locationInfoObject = locationInfoComponent.createObject(mainPageView, {
                                                                    opacity: 0
                                                                })
        locationInfoObject.anchors.bottom = mainPageView.bottom
        locationInfoObject.anchors.left = mainPageView.left
        locationInfoObject.anchors.right = mainPageView.right
        locationInfoObject.locationComponent = location
        locationInfoObject.opacity = 1
    }
}
function finishTopComponentCreation(location) {
    if (locationInfoTopComponent.status === Component.Ready) {
        locationInfoTopObject = locationInfoTopComponent.createObject(
                    mainPageView, {
                        opacity: 0
                    })
        locationInfoTopObject.anchors.top = mainPageView.top
        locationInfoTopObject.anchors.left = mainPageView.left
        locationInfoTopObject.anchors.right = mainPageView.right
        locationInfoTopObject.locationComponent = location
        locationInfoTopObject.opacity = 1

        locationInfoTopObject.requestHideBars.connect(function () {
            removeInfo()
            removeTopInfo()
        })
    }
}

function finishNavigationCreation(model, naviComponent) {
    if (navigationInfoComponent.status === Component.Ready) {
        navigationInfoObject = navigationInfoComponent.createObject(mainPageView, {
                                                                    opacity: 0,
                                                                    manuverModel: model,
                                                                    naviComponent: naviComponent
                                                                })
        navigationInfoObject.anchors.bottom = mainPageView.bottom
        navigationInfoObject.anchors.left = mainPageView.left
        navigationInfoObject.anchors.right = mainPageView.right
        navigationInfoObject.state = 'normal'
        navigationInfoObject.opacity = 1
    }
}

function finishWaypointComponent(location) {
    if (waypointComponent.status === Component.Ready) {
        waypointObject = waypointComponent.createObject(mainPageView, {
                                                                    opacity: 0
                                                                })
        waypointObject.anchors.bottom = mainPageView.bottom
        waypointObject.anchors.left = mainPageView.left
        waypointObject.anchors.right = mainPageView.right
        waypointObject.locationComponent = location
        waypointObject.opacity = 1
    }
}

function createLocationComponent(location) {
    console.debug("Creating location info")
    locationInfoComponent = Qt.createComponent("MapLocationInfo.qml")
    if (locationInfoComponent.status === Component.Ready) {
        finishComponentCreation(location)
    } else {
        console.debug(locationInfoComponent.errorString())
        locationInfoComponent.statusChanged.connect(finishComponentCreation(
                                                        location))
    }
}

function createTopInfoComponent(location) {
    console.debug('Creating top info component')
    locationInfoTopComponent = Qt.createComponent("MapLocationInfoTop.qml")
    if (locationInfoTopComponent.status === Component.Ready) {
        finishTopComponentCreation(location)
    } else {
        console.debug(locationInfoComponent.errorString())
        locationInfoTopComponent.statusChanged.connect(
                    finishTopComponentCreation(location))
    }
}

function createNavigationInstructionsItem(manuverModel, naviComponent) {
    navigationInfoComponent = Qt.createComponent(Qt.resolvedUrl("navigationview/NavigationInfo.qml"))

    if (navigationInfoComponent.status === Component.Ready) {
        finishNavigationCreation(manuverModel, naviComponent);
    } else {
        console.debug(navigationInfoComponent.errorString());
        navigationInfoComponent.statusChanged.connect(finishNavigationCreation(manuverModel, naviComponent))
    }
}

function createWaypointItem(location) {
    waypointComponent = Qt.createComponent("navigationview/WaypointItem.qml")
    if (waypointComponent.status === Component.Ready) {
        finishWaypointComponent(location)
    } else {
        console.debug(waypointComponent.errorString());
        waypointComponent.statusChanged.connect(
                    finishWaypointComponent(location))
    }
}

function remove(component, component2) {
    console.debug('Removing components', component, component2)

    if (component) {
        component.destroy()
    }
    if (component2){
        component2.destroy()
    }
}
