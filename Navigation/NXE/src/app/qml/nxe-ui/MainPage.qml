import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

import 'infoObjectLogic.js' as Info
import 'searchview'
import 'common'
import 'settingsview'

Item {
    id: mainPageView
    width: 400
    height: 800

    property var locationInfoComponent: null
    property var locationInfoTopComponent: null
    property var locationInfoObject: null
    property var locationInfoTopObject: null

    property var navigationInfoComponent: null
    property var navigationInfoObject: null

    property var waypointComponent: null
    property var waypointObject: null

    property bool alreadyInNavi: false

    property bool showBackButton: false

    property ListModel navigationManuvers: ListModel {}

    NMenu {
        id: menu
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: theme.menu.topMargin
        width: theme.menu.size.width
        height: theme.menu.size.height

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

    BackButton {
        width: 120
        height: 120
        anchors{
            top: parent.top
            left: parent.left
        }
        visible: showBackButton

        onClicked: {
            rootStack.pop();
        }
    }

    Connections {
        target: navitProxy
        onCurrentlySelectedItemChanged: {
            console.debug("onCurrentlySelectedItemChanged");
            if (navigationProxy.navigation) {
                // hide navigation info component
                navigationInfoObject.opacity = 0
            }
            Info.remove(locationInfoComponent,locationInfoObject);

            if (navitProxy.currentlySelectedItem) {
                Info.createLocationComponent(navitProxy.currentlySelectedItem)
            } else {
                if (navigationProxy.navigation) {
                    // current item is dismissed, but navigation is still in place
                    navigationInfoObject.opacity = 1
                }
            }
        }

        onWaypointItemChanged: {
            Info.remove(waypointComponent,waypointObject)
            console.debug('waypoint item', navitProxy.waypointItem)

            if(navitProxy.waypointItem) {

                // hide navigationInfoObject
                if (navigationInfoObject) {
                    navigationInfoObject.opacity = 0;
                }

                console.debug('waypoint item!')
                Info.createWaypointItem(navitProxy.waypointItem)
            } else {
                // show navigationInfoObject
                navigationInfoObject.opacity = 1;
                navitProxy.moveToCurrentPosition()
            }
        }
    }
    Connections {
        target: navigationProxy

        onNavigationChanged: {
            console.debug('navigation is', navigationProxy.navigation)
            if (!navigationProxy.navigation) {
                console.debug('Navigation stopped')
                Info.remove(locationInfoComponent,locationInfoObject);
                Info.remove(locationInfoTopComponent, locationInfoTopObject)
                Info.remove(navigationInfoComponent, navigationInfoObject)
                Info.remove(waypointComponent, waypointObject)

                alreadyInNavi = false;
            } else {
                // navigation ch
                if (alreadyInNavi) {
                    navigationInfoObject.opacity = 1;
                }

                if (locationInfoTopObject && locationInfoTopObject.locationComponent === null) {
                    // change navi component
                    console.debug("Changing top bar location component")
                    locationInfoTopObject.locationComponent = navigationProxy.naviLocation

                    navitProxy.moveToCurrentPosition();
                    return
                }

                alreadyInNavi = true;
                console.debug('start zoom to route timer')
                zoomToRouteTimer.restart()
            }
        }

        onNavigationManuver: {
            for(var i = 0; i < navigationManuvers.count; ++i) {
                navigationManuvers.get(i).active = false;
            }
            navigationManuvers.append({manuver: "turnLeft", manuverText: manuverDescription, active: true})
        }

        onNavigationFinished: {
            navigationCancellationTimer.start();
        }

        onWaypointAdded: {
            console.debug('Waypoint added')
            navitProxy.moveToCurrentPosition();
        }
    }

    Timer {
        id: navigationCancellationTimer
        running: false
        interval: 3000
        onTriggered: {
            navigationProxy.stopNavigation();
        }
    }

    Timer {
        id: zoomToRouteTimer
        running: false
        interval: 500
        onTriggered: {
            console.debug('Zoom to route')
            navitProxy.zoomToRoute();
            navitProxy.zoomBy(-2)
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

    Connections {
        target: locationInfoObject
        onNavigationUIRequested: {
            console.debug('Navi UI Requested')
            console.debug('navigation is', navigationProxy.navigation)
            Info.remove(locationInfoComponent,locationInfoObject);
            Info.remove(navigationInfoComponent, navigationInfoObject);
            console.debug('loc=', locationInfoTopObject, locationInfoTopComponent)

            if (!locationInfoTopComponent) {
                Info.createTopInfoComponent(navigationProxy.naviLocation)
            }
            locationInfoTopObject.extraInfoVisible = true;

            // clear manuver list
            navigationManuvers.clear();
            Info.createNavigationInstructionsItem(navigationManuvers, navigationProxy.naviLocation)

            navitProxy.moveToCurrentPosition();
        }
    }
}
