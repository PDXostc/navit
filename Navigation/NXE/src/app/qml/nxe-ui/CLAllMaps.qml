import QtQuick 2.0

Page {
    // TODO: Implement me
    width: 400
    height: 800

    ListView {
        anchors.fill: parent
        model: allMapsModel
        clip: true
        delegate: CLListDelegate {
            width: parent.width
            height: 50
            property string type: "empty"
            property string settingValue: ""
            property string itemText: mapName
            property var options: ListModel{}
            isDownloaded: downloaded
        }
    }
    Component.onCompleted: {
        console.debug(allMapsModel.length)
    }
}
