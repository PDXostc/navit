import QtQuick 2.0

QtObject {
    id: navitProxy
    // real (c++ wise) properties
    property bool ftu: false
    property QtObject currentlySelectedItem: null
    property QtObject waypointItem: null
    property ListModel favourites: ListModel {}
    property ListModel destinations: ListModel {}
    property int orientation: 0
    property bool topBarLocationVisible : false
    property int eta: 120
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
    function searchAddress(searchString) {
        fakeSearchTimer.start();
    }

    function searchNear(string) {
    }

    function searchSelect(what, id) {
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

    function setLocationPopUp(itemId) {
        console.debug('set popup=', itemId)
        currentlySelectedItem = fakeLocationObject;
    }

    function setTopBarVisibility(value) {
        topBarLocationVisible = value;
    }

    function setFavorite(name, favorite) {
        console.debug('Setting fav ', name, ' to ', favorite)
        fakeLocationObject.favorite = favorite;
    }

    // Real signals
    signal searchDone();
    signal gettingFavoritesDone();
    signal gettingHistoryDone();

    // fake properties
    property QtObject fakeLocationObject: QtObject {
        property string itemText: "Plac Kościuszki"
        property bool favorite: false
        property string description: "This is a description"
        property int distance: 1000
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
        interval: 1
        repeat: false
        onTriggered: {
            currentlySelectedItem = fakeLocationObject
        }
    }
    property Timer fakeWaypointTimer: Timer {
        running: false
        interval: 500
        repeat: false
        onTriggered: {
            waypointItem = fakeLocationObject
        }
    }
}
