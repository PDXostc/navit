import QtQuick 2.0
import '../common'

Page {
    id: root
    width: 380
    height: 850

    property string queryText
    property bool queryBarEnabled: true
    property string customHeader

    function model() {
        if (customHeader === "Favorites") {
            return locationFavoritesResult;
        } else {
            return locationHistoryResult;
        }
    }

    ListView {
        id: locationResultListView
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        model: root.model()
        clip: true
        delegate: LocationsResultDelegate {
            width: parent.width
            height: theme.settingsView.delHeight
            onLocationClicked: {
                navitProxy.setLocationPopUp(itemId)
                rootStack.pop()
            }
        }
    }
    ScrollBar {
        flk: locationResultListView
    }
}

