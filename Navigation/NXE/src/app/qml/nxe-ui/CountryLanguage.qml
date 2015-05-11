import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: root
    color: "black"
    width: 400
    height: 800
    state: "noDialog"

    property variant mapsToDownload: []
    property int count: 0
    property string customHeader: mapsType === 'suggested' ? "Recommended" : "Earth"

    // by default we're using suggested country model
    property var listModel: CLSuggestedCountriesModel {}

    property string mapsType: "suggested"

    function updateState() {
        if (mapsToDownload.length !== 0) {
            root.state = 'dialog'
        } else {
            root.state = 'noDialog'
        }
        count = mapsToDownload.length
    }

    function mapEntryClicked(itemText, mapSize) {
        var properMapSize = Math.ceil(mapSize / (1024 * 1024))
        var _index = mapsToDownload.indexOf(itemText)
        if (_index === -1) {
            console.debug('Map size', properMapSize)
            mapsToDownload.push(itemText)
            dialog.downloadSize += properMapSize
        } else {
            mapsToDownload.splice(_index, 1)
            dialog.downloadSize -= properMapSize
        }
        updateState()
    }

    function downloadMaps() {
        rootStack.push({
                           item: Qt.resolvedUrl("DownloadMaps.qml"),
                           replace: true,
                           properties: {
                               maps: mapsToDownload
                           }
                       })
    }

    ListView {
        id: list
        anchors.fill: parent
        model: root.listModel
        clip: true
        delegate: mapsType === "all" ? allCountriesListDelegate : suggestedCountriesListDelegate
    }

    Component {
        id: suggestedCountriesListDelegate
        CLListDelegate {
            width: parent.width
            height: 50
            isDownloaded: navitMapsProxy.isMapDownloaded(itemText)
            property real _mapSize: navitMapsProxy.mapSize(
                                        itemText) / (1024 * 1024 * 1024)

            onSubList: {
                // all maps list requested
                settingsStackView.push({
                                           item: Qt.resolvedUrl(
                                                     "CountryLanguage.qml"),
                                           properties: {
                                               listModel: allMapsModel,
                                               mapsType: "all"
                                           }
                                       })
            }

            onChecked: {
                mapEntryClicked(itemText, _mapSize)
            }
        }
    }

    Component {
        id: allCountriesListDelegate
        CLListDelegate {
            property string type: 'checkbox'
            property string itemText: mapName
            property ListModel options: ListModel {}
            width: list.width
            height: 50

            isDownloaded: downloaded
            onChecked: {
                mapEntryClicked(mapName, mapSize)
            }
        }
    }

    CLDialog {
        id: dialog
        width: parent.width
        height: 200
        numberOfDowloads: count
        onStartDownloadClicked: downloadMaps()
    }

    states: [
        State {
            name: "noDialog"
            AnchorChanges {
                target: dialog
                anchors.top: list.bottom
                anchors.bottom: undefined
            }
        },
        State {
            name: "dialog"
            AnchorChanges {
                target: dialog
                anchors.bottom: list.bottom
                anchors.top: undefined
            }
        }
    ]
    transitions: Transition {
        AnchorAnimation {
            duration: 500
        }
    }
}
