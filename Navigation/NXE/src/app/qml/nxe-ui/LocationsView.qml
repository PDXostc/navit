import QtQuick 2.0
import QtQuick.Controls 1.2

Page {
    id: root

    signal backToMapRequest
    signal showLocationRequest
    property string headerSmallText
    property var choosenLocation

    onHeaderSmallTextChanged: {
        if (headerSmallText !== "") {
            slashSymbol.text = "/"
            smallText.text = headerSmallText
        } else {
            slashSymbol.text = ""
            smallText.text = ""
        }
    }
    Column {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 10

        spacing: 10

        SettingsViewHeader {
            width: parent.width
            height: 50
            header: "Locations"
            stack: searchStackView
        }

        StackView {
            id: searchStackView
            initialItem: SettingsListView {
                id: locationsListView
                model: LocationsListModel {}
                width: parent.width
                height: parent.height - 100
                clip: true
                onSubMenuRequest: {
                    switch (url) {
                    case "LocationsHistory.qml":
                        root.busy = true;
                        navitProxy.getHistory()
                        break
                    case "LocationsFavorites.qml":
                        root.busy = true;
                        navitProxy.getFavorites()
                        break
                    default:
                        searchStackView.push(Qt.resolvedUrl(url))
                    }
                }
            }
        }
        Connections {
            target: navitProxy
            onGettingHistoryDone: {
                root.busy = false;
                searchStackView.push({
                                         item: Qt.resolvedUrl(
                                                   "LocationsHistory.qml")
                                     })
            }
            onGettingFavoritesDone: {
                root.busy = false;
                searchStackView.push({
                                         item: Qt.resolvedUrl(
                                                   "LocationsFavorites.qml")
                                     })
            }
        }
    }
}
