import QtQuick 2.0

Page {
    id: root

    Rectangle {
        id: rectangle1
        x: 0
        width: 380
        height: 140
        color: "#ffffff"
        anchors.top: parent.top
        anchors.topMargin: 10

        TextEdit {
            id: searchInput
            width: 354
            height: 132
            color: "#242424"
            text: qsTr("Search for location...")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 6
            font.pointSize: 11
            onFocusChanged: { text = ""}
        }

        Text {
            id: text1
            x: 364
            y: 117
            width: 8
            height: 18
            text: qsTr("x")
            font.pointSize: 10

            MouseArea {
                id: clearInput
                x: -4
                y: -2
                width: 17
                height: 22
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: textEdit.text = ""
            }
        }
    }

    Item {
        id: item1
        x: 96
        y: 162
        width: 150
        height: 42
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
            id: image1
            width: 150
            height: 42
            source: "search_button_bg.png"
        }
        Image {
            id: image2
            width: 16
            height: 17
            anchors.verticalCenterOffset: 0
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            source: "search_icon_white_sm.png"
        }
        Text {
            id: text2
            x: 32
            y: 13
            color: "#ffffff"
            text: qsTr("Search")
            font.pointSize: 10
        }
        MouseArea {
            id: performSearch
            width: 142
            height: 42
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                root.busy = true;
                navitProxy.search(searchInput.text)
            }
        }
    }

    Connections {
        target: navitProxy
        onSearchDone: {
            root.busy = false;
            stack.push({item: Qt.resolvedUrl("LocationsResult.qml"), properties:{queryText: searchInput.text}})
        }
    }
}

