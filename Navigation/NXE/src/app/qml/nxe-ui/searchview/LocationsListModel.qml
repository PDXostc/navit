import QtQuick 2.0

ListModel {
    id: locationsListModel

    ListElement {
        itemText: "Search"
        type: "sublist"
        options: [
            ListElement {
                text: "Search"
                url: "LocationsStructuredSearch.qml"
            }
        ]
    }
    ListElement {
        itemText: "Favorites"
        type: "sublist"
        options: [
            ListElement {
                text: "Favorites"
                url: "LocationsFavHist.qml"
            }
        ]
    }
    ListElement {
        itemText: "History"
        type: "sublist"
        options: [
            ListElement {
                text: "Destination history"
                url: "LocationsFavHist.qml"
            }
        ]
    }
}
