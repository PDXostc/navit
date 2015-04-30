import QtQuick 2.0


// Country list view
ListModel {
    id: countriesModel

    ListElement {
        itemText: "Oregon"
        type: "empty"
        options: []
        size: 100
    }
    ListElement {
        itemText: "North America"
        type: "empty"
        options: []
        size: 1000
    }
    ListElement {
        itemText: "Earth"
        type: "empty"
        options: []
        size: 2000
    }
    ListElement {
        itemText: "View All Maps"
        type: "sublist"
        options: [
            ListElement {
                url: "CLAllMaps.qml"
            }
        ]
    }
}
