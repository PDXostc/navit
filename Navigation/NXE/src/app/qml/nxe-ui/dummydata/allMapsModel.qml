import QtQuick 2.0

ListModel {
    id: allMapsModel

    ListElement {
        mapName: "Poland"
        downloaded: true
        mapSize: 1024
    }

    ListElement {
        mapName: "Germany"
        downloaded: false
        mapSize: 1024
    }
    ListElement {
        mapName: "Switzerland"
        downloaded: false
        mapSize: 1024
    }
    ListElement {
        mapName: "France"
        downloaded: false
        mapSize: 1024
    }
    ListElement {
        mapName: "Greece"
        downloaded: false
        mapSize: 1024
    }
    ListElement {
        mapName: "Finland"
        downloaded: false
        mapSize: 1024
    }
    ListElement {
        mapName: "Rivendell"
        downloaded: false
        mapSize: 1024
    }
}

