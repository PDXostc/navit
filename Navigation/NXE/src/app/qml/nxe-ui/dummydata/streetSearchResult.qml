import QtQuick 2.0

ListModel {
    id: streetSearchResult

    ListElement {
        itemText: "Albert Einstein"
        favorite: false
        description: "0.1"
        bolded: true;
    }
    ListElement {
        itemText: "Edwin Hubble"
        favorite: false
        description: "yesterday"
    }
}

