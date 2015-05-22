import QtQuick 2.0

ListModel {
    id: resultsModel

    ListElement {
        itemText: "Strasse 1"
        favorite: false
        description: "0.1"
        bolded: true;
        itemId: 1
        searchId: 0
    }
    ListElement {
        itemText: "Strasse 1"
        favorite: false
        description: "yesterday"
        itemId: 1
        searchId: 0
    }
}

