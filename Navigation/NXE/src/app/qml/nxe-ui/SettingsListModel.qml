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
        itemText: "Download Maps"
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
                url: "ManagePOIs.qml"
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
