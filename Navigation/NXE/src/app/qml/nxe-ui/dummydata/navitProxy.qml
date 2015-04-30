import QtQuick 2.0

QtObject {
    id: navitProxy
    // real (c++ wise) properties
    property bool ftu: true
    property QtObject currentlySelectedItem: null

    // Real functions
    function valueFor(settingName) {
        if (settingName === 'orientation') {
            return "North-up";
        } else if(settingName === 'enablePoi') {
            return true;
        }
    }

    function changeValueFor(settingsName, settigsValue) {
    }

    function quit() {
        Qt.quit();
    }

    function setLocationPopUp(itemName) {
        currentlySelectedItem = fakeLocationObject;
    }

    property QtObject fakeLocationObject: QtObject {
        property string name: "Plac Kościuszki"
    }
}

