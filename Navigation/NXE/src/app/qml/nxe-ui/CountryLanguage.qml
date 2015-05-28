import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Page {
    id: root
    property variant mapsToDownload: []
    property int count: 0
    property string customHeader: "Download Maps"
    busy: true


    Component.onCompleted: {
        navitMapsProxy.reloadMaps();
    }

    Connections {
        target: navitMapsProxy
        onMapsReloaded: {
            root.busy = false;
        }
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
    function updateState() {
        if (mapsToDownload.length !== 0) {
            mainItem.state = 'dialog'
        } else {
            mainItem.state = 'noDialog'
        }
        count = mapsToDownload.length
    }

    function mapEntryClicked(itemText, mapSize) {
        var properMapSize = Math.ceil(mapSize / (1024 * 1024))
        var _index = mapsToDownload.indexOf(itemText)
        if (_index === -1) {
            console.debug('Map size', properMapSize, mapSize, 'for ',itemText)
            mapsToDownload.push(itemText)
            dialog.downloadSize += properMapSize
        } else {
            mapsToDownload.splice(_index, 1)
            dialog.downloadSize -= properMapSize
        }
        updateState()
    }

    Item {
        id: mainItem
        anchors.fill: parent
        state: "noDialog"

        ListView {
            id: list
            anchors.fill: parent
            model: allMapsModel
            clip: true
            delegate: allCountriesListDelegate
        }

        ScrollBar {
            flk: list
        }

        Component {
            id: allCountriesListDelegate
            CLExpandableDelegate {
                continentText: mapName
                property ListModel options: ListModel {}
                width: list.width

                onMapEntryClicked: {
                    root.mapEntryClicked(mapName, mapSize)

                }
            }
        }

        CLDialog {
            id: dialog
            width: parent.width
            height: 400
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
}
