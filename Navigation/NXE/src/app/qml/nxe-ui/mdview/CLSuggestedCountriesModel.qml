import QtQuick 2.0


// Country list view
ListModel {
    id: countriesModel

    ListElement {
        itemText: "Oregon"
        type: "checkbox"
        options: []
    }
    ListElement {
        itemText: "North America"
        type: "checkbox"
        options: []
    }
    ListElement {
        itemText: "Earth"
        type: "checkbox"
        options: []
    }
    ListElement {
        itemText: "View All Maps"
        type: "sublist"
        options: [
            ListElement {
            }
        ]
    }
}
