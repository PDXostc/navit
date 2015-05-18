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
    property bool navigation: false
    property int distanceToDestination: 5445
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
    signal navigationStarted()
    signal navigationStopped()
    signal navigationManuver(string manuverDescription);

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
        interval: 1
        repeat: false
        onTriggered: {
//            navigation = true
            currentlySelectedItem = fakeLocationObject
        }
    }
    property Timer fakeNextManuver: Timer {
        property int count: 0
        property var manuvers: ['Turn left in 100 meters',
                                'Turn left now',
                                'Turn right in 200 meters',
                                'Turn right now',
                                'Follow the road for next 2.6 kilometers',
                                'Follow the road for next 2 kilometers',
                                'Follow the road for next 1.5 kilometers',
                                'Turn right soon',
                                'When possible, please turn around',
    ]
        running: navigation
        interval: 1000
        repeat: true
        onTriggered: {
            var str = manuvers[count++]
            navigationManuver(str)
        }
    }
}
