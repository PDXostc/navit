import QtQuick 2.0

QtObject {
    id: navitMapsProxy

    function downloadMap(mapName) {
        console.debug("Downloading map " , mapName)
        fakeCurrMapDownload = mapName
        fakeTimer.downloaded = 0;
        fakeTimer.restart();
    }

    function isMapDownloaded(mapName) {
//        return mapName === 'Oregon' || mapName === 'Poland';
        return false
    }

    function mapSize(name) {
        return 9 * 1024 * 1024 * 1024;
    }

    function cancelDownload() {
        mapDownloadError('Oregon', 'Canceled')
        fakeTimer.stop()
    }
    function reloadMaps() {
    }

    function requestMapsReload() {
        mapsReloaded()
    }

    // real signals
    signal mapDownloadProgress(string map, int now, int total);
    signal mapDownloadError(string map, string error);
    signal mapDownloadFinished(string map);
    signal mapsReloaded()

    // Fake properties
    property Timer fakeTimer: Timer {
        interval: 1000
        property real total: 150 * 1024 * 1024
        property real delta: 1024 * 1024 * 7
        property real downloaded: 0
        repeat: true
        onTriggered: {
            console.debug('asd')
            downloaded += delta;
            if (downloaded < total) {
                console.debug('Downloaded=', downloaded, " out of ", total)
                mapDownloadProgress(fakeCurrMapDownload, downloaded, total)
            } else {
                console.debug('map downloaded', fakeCurrMapDownload);
                mapDownloadFinished(fakeCurrMapDownload)
                fakeTimer.stop();
            }
        }
    }

    property string fakeCurrMapDownload
}

