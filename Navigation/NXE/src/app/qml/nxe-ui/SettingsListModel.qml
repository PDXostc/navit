import QtQuick 2.0

ListModel {
    id: settingsListModel

    ListElement {
        itemText: "Point of Interest"
        type: "text"
        settingsValue: "poi"
        options: [
            ListElement {
                option: "on"
            },
            ListElement {
                option: "off"
            }
        ]
    }

    ListElement {
        itemText: "Orientation"
        type: "text"
        options: [
            ListElement {
                option: "north-up"
            },
            ListElement {
                option: "follow"
            }
        ]
    }
    ListElement {
        itemText: "2D/3D View"
        type: "text"
        options: [
            ListElement {
                option: "2D"
            },
            ListElement {
                option: "3D"
            }
        ]
    }
    ListElement {
        itemText: "Theme"
        type: "text"
        options: [
            ListElement {
                option: "basic"
            }
        ]
    }
    ListElement {
        itemText: "Fast/Short Routes"
        type: "text"
        options: [
            ListElement {
                option: "basic"
            }
        ]
    }
    ListElement {
        itemText: "Voice Navigation"
        type: "text"
        options: [
            ListElement {
                option: "on"
            },
            ListElement {
                option: "off"
            }
        ]
    }

    ListElement {
        itemText: "Country/Langugage"
        type: "sublist"
        options: [
            ListElement {
                url: "CountryLanguage.qml"
            }
        ]
    }
    ListElement {
        itemText: "Manage POIs"
        type: "sublist"
        options: [
            ListElement {
                url: "CountryLanguage.qml"
            }
        ]
    }
    ListElement {
        itemText: "Delete Cache"
        type: "empy"
        options: []
    }
    ListElement {
        itemText: "Factory Reset"
        type: "empy"
        options: []
    }
}
