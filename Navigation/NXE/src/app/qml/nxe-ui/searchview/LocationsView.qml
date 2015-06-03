import QtQuick 2.0
import QtQuick.Controls 1.2
import '../common'
import '../settingsview'

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
            height: theme.settingsHeaderView.height
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
                        if (itemText === "Favorites") {
                            navitProxy.getFavorites();
                            enabled = (locationFavoritesResult.length !== 0)
                        } else if (itemText === "History") {
                            navitProxy.getHistory();
                            enabled = (locationHistoryResult.length !== 0)
                        }
                    }
                    width: locationsListView.width
                    height: theme.settingsView.delHeight
                    onSubMenuRequested: {
                        console.debug('request ', Qt.resolvedUrl(url))
                        searchStackView.push({
                                                 item: Qt.resolvedUrl(url),
                                                 properties: {customHeader: itemText}
                                             })
                    }
                    opacity: enabled ? 1 : 0.4
                }
            }

            onCurrentItemChanged: {
                if (currentItem && currentItem.customHeader) {
                    header.subHeader = currentItem.customHeader
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
