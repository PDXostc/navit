import QtQuick 2.0
import QtQml 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Page {
    id: root

    property string searchForWhat: 'country'
    property string selected
    property string currentLocation
    property int numberOfCharsToStartSearch: 2

    function startPredictiveSearch(string) {
        console.debug('start searching')
        root.busy = true
        if (searchForWhat === 'country') {
            navitProxy.searchCountry(string)
        } else if (searchForWhat === 'city') {
            navitProxy.searchCity(string)
        } else if(searchForWhat === 'street') {
            navitProxy.searchStreet(string)
        }
    }

    Component.onCompleted: {
        if (searchForWhat === 'country') {
            navitProxy.startSearch()
        }
    }

    Column {
        anchors.fill: parent

        Item {
            width: parent.width
            height: 30
            Text {
                text: selected
                anchors.right: parent.right
                color: "white"
                font.pixelSize: 16
            }
        }

        Item {
            width: parent.width
            height: 24
            RowLayout {
                id: rowLayout
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10

                Text {
                    text: searchForWhat
                    color: "white"
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: parent.height
                    color: "white"

                    TextEdit {
                        id: searchInput
                        color: "#242424"
                        font.pixelSize: 17
                        anchors.fill: parent
                        onTextChanged: {
                            if (text.length >= numberOfCharsToStartSearch) {
                                startPredictiveSearch(text)
                            }
                        }
                    }
                }
            }

            Item {
                id: resultListViewItem
                width: searchInput.width
                height: 0
                anchors.top: rowLayout.bottom
                anchors.topMargin: 5
                anchors.right: rowLayout.right

                Behavior on height{ NumberAnimation{}}

                ListView {
                    anchors.fill: parent
                    model: {
                        if (searchForWhat === 'country') {
                            return countrySearchResult
                        } else if (searchForWhat === 'city') {
                            return citySearchResult
                        } else if (searchForWhat === 'street') {
                            return streetSearchResult;
                        }
                    }
                    clip: true
                    delegate: MouseArea {
                        width: resultListViewItem.width
                        height: 40

                        onClicked: {
                            var nextSearch
                            if (searchForWhat === 'country') {
                                nextSearch = 'city'
                            } else if (searchForWhat === 'city') {
                                nextSearch = 'street'
                            } else if(searchForWhat === 'street') {
                                nextSearch = 'address';
                            }

                            var headerText
                            if (selected.length !== 0) {
                                headerText = itemText + ", " + selected;
                            } else {
                                headerText = itemText;
                            }

                            searchStackView.push({
                                                     item: Qt.resolvedUrl(
                                                               "LocationsStructuredSearch.qml"),
                                                     properties: {
                                                         searchForWhat: nextSearch,
                                                         selected: headerText,
                                                         currentLocation: itemText
                                                     }
                                                 })
                        }

                        Text {
                            text: itemText
                            color: "white"
                            font.pixelSize: 16
                        }

                        Rectangle {
                            width: parent.width
                            height: 1
                            anchors.bottom: parent.bottom
                        }
                    }
                }
            }
        }
    }

    NButton {
        text: "Show on the map"
        visible: searchForWhat !== 'country' && searchForWhat !=='city'
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            navitProxy.setLocationPopUp(currentLocation);
            rootStack.pop();
        }
    }

    Connections {
        target: navitProxy
        // @disable-check M16
        onSearchDone: {
            root.busy = false
            resultListViewItem.height = 200
            Qt.inputMethod.hide();
        }
    }
}

