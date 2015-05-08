import QtQuick 2.0

ListModel {
    id: resultsModel

    ListElement {
        itemText: "Munchen"
        favorite: false
        description: "0.1"
        bolded: true;
    }
    ListElement {
        itemText: "Berlin"
        favorite: false
        description: "yesterday"
    }
}

