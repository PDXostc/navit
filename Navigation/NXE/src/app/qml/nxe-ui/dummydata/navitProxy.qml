import QtQuick 2.0

QtObject {
    id: navitProxy
    // real (c++ wise) properties
    property bool ftu: false
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

    function downloadMap(mapName) {
        fakeTimer.restart();
    }

    function quit() {
        Qt.quit();
    }

    function setLocationPopUp(itemName) {
        currentlySelectedItem = fakeLocationObject;
    }

    // real signals
    signal mapDownloadProgress(string map, int now, int total);
    signal mapDownloadError(string map, string error);
    signal mapDownloadFinished(string map);

    // Fake properties
    property Timer fakeTimer: Timer {
        interval: 1000
        onTriggered: mapDownloadFinished('Oregon')
    }
    property QtObject fakeLocationObject: QtObject {
        property string name: "Plac Kościuszki"
    }
}

