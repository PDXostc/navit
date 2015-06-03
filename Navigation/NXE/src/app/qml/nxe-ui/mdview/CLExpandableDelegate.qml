import QtQuick 2.0
import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import '../common'

Item {
    id: root
    property bool isExpanded : false
    property string continentText
    property bool isDownloaded: false
    property var mapsModel
    property int mapsCount : 0
    signal checked()
    signal mapEntryClicked(string mapName, int mapSize)

    Component.onCompleted: {
        switch (continentText) {
        case "Downloaded Maps":
            mapsModel = downloadedMapsModel
            break
        case "Recommended Maps":
            mapsModel = recommendedMapsModel
            break
        case "Earth":
            mapsModel = earthModel
            break
        case "Africa":
            mapsModel = africaModel
            break
        case "Asia":
            mapsModel = asiaModel
            break
        case "Australia":
            mapsModel = australiaModel
            break
        case "Europe":
            mapsModel = europeModel
            break
        case "North America":
            mapsModel = northAmericaModel
            break
        case "South+Middle America":
            mapsModel = southMiddleAmericaModel
            break
        }
        for(var _mapsModel in mapsModel) mapsCount += 1;
    }
    width: 400
    height: isExpanded ? mapsCount * 70 + 70 : 70

    Rectangle {
        width: parent.width
        height: 1
        color: "#45a6c3"
        anchors.bottom: parent.bottom
        visible: !isExpanded
    }

    Image {
        id: openedIcon
        width: isExpanded ? 16 : 8
        height: isExpanded ? 8 : 16
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 30
        source: isExpanded ? "../images/list_toggle_open_white.png" : "../images/list_toggle_closed_white.png"
    }

    NText {
        id: groupName
        color: "#ffffff"
        text: continentText.replace("+", " and ")
        anchors.verticalCenter: parent.verticalCenter
        anchors.top: parent.top
        anchors.topMargin: 18
        anchors.left: parent.left
        anchors.leftMargin: 32
    }

    MouseArea {
        width: parent.width
        height: parent.height
        onClicked: {
            isExpanded = !isExpanded
        }
    }

    ListView {
        id: groupedMaps
        model: mapsModel
        delegate: expandedDelegate
        height: parent.height - 50
        anchors.bottom: parent.bottom
        visible: isExpanded
    }

    Component {
        id: expandedDelegate
        CLExpandedDelegate {
            height: 70
            width: root.width
            itemText: mapName
            itemSize: mapSize
            isDownloaded: downloaded
            onChecked: {
                root.mapEntryClicked(mapName, mapSize)
            }
        }
    }

    ScrollBar {
        flk: groupedMaps
    }
}
