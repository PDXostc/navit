import QtQuick 2.0

ListModel {
    id: settingsListModel

    ListElement {
        itemText: "Point of Interest"
        type: "text"
        settingsValue: "enablePoi"
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
        settingsValue: "orientation"
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
        settingsValue: "perspective"
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
        settingValue: "theme"
        options: [
            ListElement {
                option: "basic"
            }
        ]
    }
    ListElement {
        itemText: "Fast/Short Routes"
        type: "text"
        settingsValue: "routeType"
        options: [
            ListElement {
                option: "basic"
            }
        ]
    }
    ListElement {
        itemText: "Voice Navigation"
        type: "text"
        settingsValue: "voice"
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
                url: ""
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
        type: "popup"
        options: [
            ListElement {
                url: "FactoryResetDialog.qml"
            }
        ]
    }
}
