import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

import 'infoObjectLogic.js' as Info

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

    property ListModel navigationManuvers: ListModel {}


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
            if (navitProxy.navigation) {
            } else {
                Info.remove(locationInfoComponent,locationInfoObject);
                Info.remove(locationInfoTopComponent, locationInfoTopObject)

                if (navitProxy.currentlySelectedItem) {
                    Info.createLocationComponent(navitProxy.currentlySelectedItem)
                }
            }
        }

        onNavigationChanged: {
            console.debug('navigation is', navitProxy.navigation)
            if (navitProxy.navigation) {
                console.debug('navigation=', navitProxy.navigation);
                Info.remove(locationInfoComponent,locationInfoObject);


                if (!locationInfoTopComponent) {
                    Info.createTopInfoComponent(null)
                }
                locationInfoTopObject.extraInfoVisible = true;

                // clear manuver list
                navigationManuvers.clear();
                Info.createNavigationInstructionsItem(navigationManuvers);
            } else {
                console.debug('Navigation stopped')
                Info.remove(locationInfoComponent,locationInfoObject);
                Info.remove(locationInfoTopComponent, locationInfoTopObject)
                Info.remove(navigationInfoComponent, navigationInfoObject)
            }
        }

        onNavigationManuver: {
            for(var i = 0; i < navigationManuvers.count; ++i) {
                navigationManuvers.get(i).active = false;
            }
            navigationManuvers.append({manuver: "turnLeft", manuverText: manuverDescription, active: true})
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
