import QtQuick 2.0
import QtQuick.Controls 1.2

Page {
    id: root

    signal backToMapRequest
    signal showLocationRequest
    property var choosenLocation

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
            id: header
        }

        StackView {
            id: searchStackView
            initialItem: SettingsListView {
                id: locationsListView
                model: LocationsListModel {}
                width: parent.width
                height: parent.height - 100
                clip: true
                delegate: SettingsListDelegate {
                    Component.onCompleted: {
                        console.debug(options.get(0).url)
                        if (options.get(0).url === 'LocationsFavorites.qml') {
                            navitProxy.getFavorites();
                            enabled = (locationFavoritesResult.length !== 0)
                        } else if (options.get(0).url === 'LocationsHistory.qml') {
                            navitProxy.getHistory();
                            enabled = (locationHistoryResult.length !== 0)
                        }
                    }
                    width: locationsListView.width
                    height: 50
                    onSubMenuRequested: {
                        console.debug('request ', Qt.resolvedUrl(url))
                        searchStackView.push(Qt.resolvedUrl(url))
                    }
                    opacity: enabled ? 1 : 0.4
                }
            }

            onCurrentItemChanged: {
                if (currentItem && currentItem.subHeader) {
                    header.subHeader = currentItem.subHeader
                } else {
                    header.subHeader = "";
                }
            }
        }
        Connections {
            target: navitProxy
        }
    }
}
