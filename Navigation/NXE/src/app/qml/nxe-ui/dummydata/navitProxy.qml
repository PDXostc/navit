import QtQuick 2.0

QtObject {
    id: navitProxy
    // real (c++ wise) properties
    property bool ftu: false
    property QtObject currentlySelectedItem: null
    property ListModel favourites: ListModel {}
    property ListModel destinations: ListModel {}

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

    function search(searchString) {
        fakeSearchTimer.start();
    }

    function quit() {
        Qt.quit();
    }

    function setLocationPopUp(itemName) {
        currentlySelectedItem = fakeLocationObject;
    }

    function setFavorite(name, favorite) {
        console.debug('Setting fav ', name, ' to ', favorite)
        fakeLocationObject.favorite = favorite;
    }

    // Real signals

    signal searchDone();

    // fake properties
    property QtObject fakeLocationObject: QtObject {
        property string itemText: "Plac Kościuszki"
        property bool favorite: false
        property string description: "This is a description"
    }

    property Timer fakeSearchTimer: Timer {
        running: false
        interval: 3000
        onTriggered: {
            searchDone()
        }
    }
}

