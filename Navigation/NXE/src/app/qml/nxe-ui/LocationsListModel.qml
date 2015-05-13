import QtQuick 2.0

ListModel {
    id: locationsListModel

    ListElement {
        itemText: "Search"
        type: "sublist"
        enabled: true
        options: [
            ListElement {
                text: "Search"
                url: "LocationsSearch.qml"
            }
        ]
    }
    ListElement {
        itemText: "Structured Search"
        type: "sublist"
        enabled: true
        options: [
            ListElement {
                text: "StructureSearch"
                url: "LocationsStructuredSearch.qml"
            }
        ]
    }
    ListElement {
        itemText: "Favorites"
        type: "sublist"
        enabled: false
        options: [
            ListElement {
                text: "Favorites"
                url: "LocationsFavorites.qml"
            }
        ]
    }
    ListElement {
        itemText: "Destination history"
        type: "sublist"
        enabled: false
        options: [
            ListElement {
                text: "Destination history"
                url: "LocationsHistory.qml"
            }
        ]
    }
}
