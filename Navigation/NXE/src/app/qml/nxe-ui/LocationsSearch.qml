import QtQuick 2.0
import QtQml 2.2

Page {
    id: root

    property string searchForWhat: 'country'

    Component.onCompleted: {
        if (searchForWhat === 'country') {
            navitProxy.startSearch();
        }
        console.debug('Searching for', searchForWhat);
    }

    Rectangle {
        id: rectangle1
        x: 0
        width: 380
        height: 140
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10

        TextEdit {
            id: searchInput
            width: 354
            height: 132
            color: "#242424"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 6
            font.pixelSize: 11
            text: "Search for " + searchForWhat
            onFocusChanged: {text = ""}
        }

        Text {
            id: text1
            x: 364
            y: 117
            width: 8
            height: 18
            text: qsTr("x")
            font.pixelSize: 10

            MouseArea {
                id: clearInput
                x: -4
                y: -2
                width: 17
                height: 22
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: searchInput.text = ""
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
            font.pixelSize: 10
        }
        MouseArea {
            id: performSearch
            width: 142
            height: 42
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                // for some reasons weekeyboard needs to be manually hidden
                Qt.inputMethod.hide();
                searchInput.focus = false;
                root.busy = true;
                if (searchForWhat === 'country') {
                    navitProxy.searchCountry(searchInput.text)
                } else if(searchForWhat === 'city') {
                    navitProxy.searchCity(searchInput.text)
                }
            }
        }
    }

    Connections {
        target: navitProxy
        // @disable-check M16
        onSearchDone: {
            root.busy = false;
            searchStackView.push({
                           item: Qt.resolvedUrl("LocationsResult.qml"),
                           properties:{
                               queryText: searchInput.text,
                               whatIsSearched: searchForWhat
                           }
                       })
        }
    }
}

