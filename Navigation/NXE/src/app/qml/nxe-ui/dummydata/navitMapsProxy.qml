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
        property real total: 150 * 1024 * 1024
        property real delta: 1024 * 1024 * 7
        property real downloaded: 0
        repeat: true
        onTriggered: {
            downloaded += delta;
            if (downloaded < total) {
                console.debug('Downloaded=', downloaded, " out of ", total)
                mapDownloadProgress('Oregon', downloaded, total)
            } else {
                mapDownloadFinished('Oregon')
                fakeTimer.stop();
            }
        }
    }
}

