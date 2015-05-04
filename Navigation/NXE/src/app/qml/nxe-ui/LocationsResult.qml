import QtQuick 2.0

Rectangle {
    width: 380
    height: 850
    color: "#000000"

    property string queryText
    property bool queryBarEnabled: true
    Item {
        id: queryBar
        x: 96
        y: 8
        width: 446
        height: queryBarEnabled ? 42 : 0
        visible: true
        anchors.horizontalCenterOffset: 33
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
            id: image1
            y: 0
            width: 422
            height: parent.height
            visible: true
            anchors.left: parent.left
            anchors.leftMargin: 0
            source: "search_button_bg.png"
        }
        Image {
            id: image2
            width: 16
            height: queryBarEnabled ? 16 : 0
            anchors.verticalCenterOffset: 0
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            source: "search_icon_white_sm.png"
        }
        Text {
            x: 32
            y: 13
            color: "#ffffff"
            text: queryBarEnabled ? queryText : ""
            font.pointSize: 10
        }
    }


    ListView {
        id: locationResultListView
        height: 800
        anchors.top: queryBarEnabled ? queryBar.bottom : parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: queryBarEnabled ? 7 : 0
        anchors.bottomMargin: 0
        model: locationSearchResult
        clip: true
        delegate: LocationsResultDelegate {
            width: parent.width
            height: 50
        }
    }
    ScrollBar {
        flk: locationResultListView
    }

    Rectangle {
        id: rectangle1
        x: 380
        y: 0
        width: 42
        height: 53
        color: "#000000"
    }
}
