import QtQuick 2.0

ListModel {
    id: allMapsModel

    ListElement {
        mapName: "All Earth"
        downloaded: false
        mapSize: 1024
    }

    ListElement {
        mapName: "Africa"
        downloaded: false
        mapSize: 1024
    }
    ListElement {
        mapName: "Asia"
        downloaded: false
        mapSize: 1024
    }
}

