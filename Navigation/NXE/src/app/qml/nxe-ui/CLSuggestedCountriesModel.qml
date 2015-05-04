import QtQuick 2.0


// Country list view
ListModel {
    id: countriesModel

    ListElement {
        itemText: "Oregon"
        type: "empty"
        options: []
    }
    ListElement {
        itemText: "North America"
        type: "empty"
        options: []
    }
    ListElement {
        itemText: "Earth"
        type: "empty"
        options: []
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
