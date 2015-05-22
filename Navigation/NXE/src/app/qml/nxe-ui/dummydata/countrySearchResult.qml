import QtQuick 2.0

ListModel {
    id: resultsModel

    ListElement {
        itemText: "Germany"
        favorite: false
        description: "0.1"
        bolded: true;
        searchId: 0
        itemId: 1
    }
    ListElement {
        itemText: "Poland"
        favorite: false
        description: "yesterday"
        searchId: 0
        itemId: 2
    }
}

