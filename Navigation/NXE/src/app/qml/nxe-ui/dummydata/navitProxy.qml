import QtQuick 2.0

QtObject {
    id: navitProxy
    property bool ftu: false

    // Fake properties
    property Timer fakeTimer: Timer {
        interval: 1000
        onTriggered: mapDownloadFinished('Oregon')
    }
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

    signal mapDownloadProgress(string map, int now, int total);
    signal mapDownloadError(string map, string error);
    signal mapDownloadFinished(string map);

}

