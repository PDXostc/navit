import QtQuick 2.0

QtObject {
    id: navitMapsProxy

    function downloadMap(mapName) {
        fakeTimer.restart();
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
}

