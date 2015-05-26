import QtQuick 2.0

Page {
    id: root
    property string mapName

    Connections {
        target: navitMapsProxy
        onMapDownloadError: {
            console.error("An error during map download ", error)
            if (error === 'Canceled') {
                root.busy = false;
                rootStack.clear();
                rootStack.push({
                           item: Qt.resolvedUrl("NxeUI.qml")
                       })
                navitProxy.quit();
            }
        }
    }

    NDialog {
        id: dialogItem
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        headerText: "Are you sure?"
        dialogText: "All maps currently downloaded will be removed."
        bottomBarComponent: Row {
            width: dialogItem.width
            height: 60
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 5

            NButton {
                id: cancelItem
                text: "Quit"
                iconSource: "next_icon_white.png"
                onClicked: {
                    root.busy = true;
                    navitMapsProxy.cancelDownload(mapName);
                }
            }

            Item {
                width: parent.width - factoryResetItem.width - cancelItem.width - 10
                height: parent.height
            }

            NButton {
                id: factoryResetItem
                text: "Continue Downloading"
                iconSource: "next_icon_white.png"
                onClicked: rootStack.pop()
            }
        }
    }
}

