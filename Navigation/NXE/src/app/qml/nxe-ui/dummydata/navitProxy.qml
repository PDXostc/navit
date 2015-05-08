import QtQuick 2.0

QtObject {
    id: navitProxy
    // real (c++ wise) properties
    property bool ftu: false
    property QtObject currentlySelectedItem: null
    property ListModel favourites: ListModel {}
    property ListModel destinations: ListModel {}
    property bool topBarLocationVisible : false
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

    function startSearch() {
    }

    function searchCountry(searchString) {
        fakeSearchTimer.start();
    }

    function searchCity(searchString) {
        fakeSearchTimer.start();
    }

    function getHistory() {
        fakeHistoryTimer.start();
    }

    function getFavorites() {
        fakeFavoritesTimer.start();
    }

    function quit() {
        Qt.quit();
    }

    function setLocationPopUp(itemName) {
        currentlySelectedItem = fakeLocationObject;
    }

    function setTopBarVisibility(value) {
            topBarLocationVisible = value;
    }

    function hideLocationBars() {
        currentlySelectedItem = null
        topBarLocationVisible = false;
    }

    function setFavorite(name, favorite) {
        console.debug('Setting fav ', name, ' to ', favorite)
        fakeLocationObject.favorite = favorite;
    }

    // Real signals

    signal searchDone();
    signal gettingFavoritesDone();
    signal gettingHistoryDone();
    signal pointClicked(var location)

    // fake properties
    property QtObject fakeLocationObject: QtObject {
        property string itemText: "Plac Kościuszki"
        property bool favorite: false
        property string description: "This is a description"
    }

    property Timer fakeSearchTimer: Timer {
        running: false
        interval: 1000
        onTriggered: {
            searchDone();
        }
    }

    property Timer fakeFavoritesTimer: Timer {
        running: false
        interval: 3000
        onTriggered: {
            gettingFavoritesDone()
        }
    }

    property Timer fakeHistoryTimer: Timer {
        running: false
        interval: 3000
        onTriggered: {
            gettingHistoryDone()
        }
    }

    property Timer fakePositionClickedTimer: Timer {
        running: false
        interval: 1000
        repeat: false
        onTriggered: {
            pointClicked(fakeLocationObject)
        }
    }
}
