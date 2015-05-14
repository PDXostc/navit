import QtQuick 2.0

QtObject {
    id: navitProxy
    // real (c++ wise) properties
    property bool ftu: false
    property QtObject currentlySelectedItem: null
    property ListModel favourites: ListModel {}
    property ListModel destinations: ListModel {}
    property int orientation: 0
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

    function finishSearch() {
    }

    function searchCountry(searchString) {
        fakeSearchTimer.start();
    }

    function searchCity(searchString) {
        fakeSearchTimer.start();
    }

    function searchStreet(searchString) {
        fakeSearchTimer.start();
    }

    function searchNear(string) {
    }

    function getHistory() {
        fakeHistoryTimer.start();
    }

    function getFavorites() {
        fakeFavoritesTimer.start();
    }

    function moveToCurrentPosition() {
    }

    function reset() {
        ftu = true
    }

    function quit() {
        Qt.quit();
    }

    function setLocationPopUp(itemName) {
        console.debug('set popup=', itemName)
        currentlySelectedItem = fakeLocationObject;
    }

    function setTopBarVisibility(value) {
            topBarLocationVisible = value;
    }

    function setFavorite(name, favorite) {
        console.debug('Setting fav ', name, ' to ', favorite)
        fakeLocationObject.favorite = favorite;
    }

    function startNavigation() {
        navigationStarted()
    }

    function cancelNavigation() {
        navigationStopped()
    }

    // Real signals

    signal searchDone();
    signal gettingFavoritesDone();
    signal gettingHistoryDone();
    signal pointClicked(var location)
    signal navigationStarted()
    signal navigationStopped()

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
        running: true
        interval: 1000
        repeat: false
        onTriggered: {
            pointClicked(fakeLocationObject)
        }
    }
}
