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

    function updateState() {
        if (mapsToDownload.length !== 0) {
            root.state = 'dialog'
        } else {
            root.state = 'noDialog'
        }
        count = mapsToDownload.length
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
        model: CLSuggestedCountriesModel {}
        clip: true
        delegate: CLListDelegate {
            width: parent.width
            height: 50

            onSubList: {
                settingsStackView.push(Qt.resolvedUrl(url))
            }

            onChecked: {
                var _index = mapsToDownload.indexOf(itemText);
                if (_index === -1) {
                    mapsToDownload.push(itemText)
                    dialog.downloadSize += size
                } else {
                    mapsToDownload.splice(_index, 1)
                    dialog.downloadSize -= size
                }

                updateState()
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
                anchors.top: root.bottom
                anchors.bottom: undefined
            }
        },
        State {
            name: "dialog"
            AnchorChanges {
                target: dialog
                anchors.bottom: root.bottom
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
