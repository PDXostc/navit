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

        locationInfoTopObject.requestHideBars.connect(function () {
            removeInfo()
            removeTopInfo()
        })
    }
}

function finishNavigationCreation(model) {
    if (navigationInfoComponent.status === Component.Ready) {
        navigationInfoObject = navigationInfoComponent.createObject(mainPageView, {
                                                                    opacity: 0,
                                                                    manuverModel: model
                                                                })
        navigationInfoObject.anchors.bottom = mainPageView.bottom
        navigationInfoObject.anchors.left = mainPageView.left
        navigationInfoObject.anchors.right = mainPageView.right
        navigationInfoObject.state = 'normal'
        navigationInfoObject.opacity = 1
    }
}

function createLocationComponents(location) {
    createLocationComponent(location)
    createTopInfoComponent(location)
}

function createLocationComponent(location) {
    locationInfoComponent = Qt.createComponent("MapLocationInfo.qml")
    if (locationInfoComponent.status === Component.Ready) {
        finishComponentCreation(location)
    } else {
        locationInfoComponent.statusChanged.connect(finishComponentCreation(
                                                        location))
    }
}

function createTopInfoComponent(location) {
    locationInfoTopComponent = Qt.createComponent("MapLocationInfoTop.qml")
    if (locationInfoTopComponent.status === Component.Ready) {
        finishTopComponentCreation(location)
    } else {
        locationInfoTopComponent.statusChanged.connect(
                    finishTopComponentCreation(location))
    }
}

function createNavigationInstructionsItem(manuverModel) {
    navigationInfoComponent = Qt.createComponent(Qt.resolvedUrl("NavigationInfo.qml"))

    if (navigationInfoComponent.status === Component.Ready) {
        finishNavigationCreation(manuverModel);
    } else {
        navigationInfoComponent.statusChanged.connect(finishNavigationCreation(manuverModel))
    }
}

function remove(component, component2) {

    if (component) {
        component.destroy()
    }
    if (component2){
        component2.destroy()
    }
}
