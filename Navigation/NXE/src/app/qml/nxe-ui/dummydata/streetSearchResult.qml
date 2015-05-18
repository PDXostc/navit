import QtQuick 2.0

ListModel {
    id: streetSearchResult

    ListElement {
        itemText: "Albert Einstein"
        favorite: false
        description: "0.1"
        bolded: true;
        itemId: 1
        searchId: 0
    }
    ListElement {
        itemText: "Edwin Hubble"
        favorite: false
        description: "yesterday"
        itemId: 1
        searchId: 0
    }
}

