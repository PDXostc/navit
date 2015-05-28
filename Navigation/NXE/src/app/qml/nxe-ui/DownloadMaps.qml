import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Page {

    property var maps
    property string currentDownloadMap
    property int currentDownloadIndex: 0

    property real bytesDownloaded: 0
    property real sumBytes: 0
    property real totalBytes: 0

    Component.onCompleted: {
        currentDownloadMap = maps[currentDownloadIndex]
        navitMapsProxy.downloadMap(currentDownloadMap)
        etaCalculationTimer.start()
    }

    function downloadNextMap() {
        currentDownloadIndex++
        progressBarItem.value = 0

        if (currentDownloadIndex === maps.length) {
            // push FTUMapDownloadCompleted.qml
            rootStack.push({
                               item: Qt.resolvedUrl("DownloadCompleted.qml"),
                               replace: true
                           })
        } else {
            currentDownloadMap = maps[currentDownloadIndex]
            navitMapsProxy.downloadMap(currentDownloadMap)
            etaCalculationTimer.restart()
        }
    }

    Timer {
        id: etaCalculationTimer
        running: false
        interval: 1000
        repeat: true
        property int numberOfSeconds: 0
        onTriggered: {
            sumBytes += bytesDownloaded
            if (sumBytes === 0)
                return
            numberOfSeconds++

            var allSeconds = totalBytes * numberOfSeconds / bytesDownloaded
            var secondsLeft = allSeconds - numberOfSeconds
            console.debug('All sec = ', allSeconds)
            console.debug('Left = ', secondsLeft)

            etaTextItem.text = "" + Math.ceil(secondsLeft) + " sec"
        }
    }

    Connections {
        target: navitMapsProxy
        onMapDownloadError: {
            console.error("An error during map download ", error)
        }

        onMapDownloadProgress: {
            var progress = now / total
            console.debug("Progress for " + map + " value=" + progress)
            var nowInt = parseInt(now, 10)
            var totalInt = parseInt(total, 10)
            if (map === currentDownloadMap) {
                progressBarItem.value = progress
                bytesTextItem.text = Math.floor(
                            now / (1024 * 1024)) + "MB/" + Math.floor(
                            total / (1024 * 1024)) + "MB"

                bytesDownloaded = now
                totalBytes = total
            }
        }

        onMapDownloadFinished: {
            etaCalculationTimer.stop()
            downloadNextMap()
        }
    }

    Rectangle {
        id: rect
        width: parent.width
        height: 450
        color: "#09bcdf"
        anchors.verticalCenter: parent.verticalCenter
    }

    Image {
        id: icon
        anchors.bottomMargin: -96/2
        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.height: 96
        sourceSize.width: 96
        source: "download_hex_icon.png"
        anchors.bottom: rect.top
    }

    Column {
        spacing: 30
        anchors {
            top: icon.bottom
            topMargin: 10
            bottom: rect.bottom
            left: rect.left
            leftMargin: 10
            right: rect.right
            rightMargin: 10
        }
        Text {
            id: headerText
            color: "white"
            text: "Downloading (" + (currentDownloadIndex + 1) + " of " + maps.length + ")"
            font.pixelSize: 40
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ProgressBar {
            id: progressBarItem
            width: parent.width
            height: 40
            value: 0
            Behavior on value {
                NumberAnimation {
                }
            }
            style: ProgressBarStyle {
                background: Rectangle {
                    radius: 2
                    color: "#467C8A"
                    border.color: "#467C8A"
                    border.width: 1
                    implicitWidth: 200
                    implicitHeight: 8
                }
                progress: Rectangle {
                    color: "#BABABA"
                    border.color: "#BABABA"
                }
            }
        }

        Item {
            width: parent.width
            height: 20

            NText {
                text: currentDownloadMap
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            NText {
                id: bytesTextItem
                text: "N/A"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item {
            width: parent.width
            height: 20

            NText {
                text: "Time remaining"
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            NText {
                id: etaTextItem
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    NButton {
        text: "Cancel"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: rect.bottom
        iconType: "arrow_right"
        iconHeight: 30
        onClicked: {
            rootStack.push({
                               item: Qt.resolvedUrl(
                                         "DownloadCancelConfirmationDialog.qml"),
                               properties: {
                                   mapName: currentDownloadMap
                               }
                           })
        }
    }
}
