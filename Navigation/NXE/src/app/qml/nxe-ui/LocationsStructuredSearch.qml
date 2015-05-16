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
    property bool searchInProgress: false

    // Hack for weekeyboard
    property string currentSearchString

    function startPredictiveSearch(string) {
        if (root.busy)
            return;
        listViewItem.model = null;
        root.busy = true;
        currentSearchString = string
        console.debug('start searching for ', currentSearchString)
        if (searchForWhat === 'country') {
            navitProxy.searchCountry(string)
        } else if (searchForWhat === 'city') {
            navitProxy.searchCity(string)
        } else if(searchForWhat === 'street') {
            navitProxy.searchStreet(string)
        } else if(searchForWhat === 'address') {
            navitProxy.searchAddress(string)
        }
    }

    Component.onCompleted: {
        if (searchForWhat === 'country') {
            navitProxy.startSearch()
        } else if (searchForWhat == 'address') {
            startPredictiveSearch('');
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
                            console.debug('text has changed')
                            if (searchForWhat == 'address') {
                                startPredictiveSearch(text);
                            } else {
                                if (text.length >= numberOfCharsToStartSearch && currentSearchString != text) {
                                    startPredictiveSearch(text)
                                }
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

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "white"
                }

                Behavior on height{ NumberAnimation{}}

                ListView {
                    id: listViewItem
                    anchors.fill: parent
                    model: {
                    }

                    clip: true
                    delegate: MouseArea {
                        width: resultListViewItem.width
                        height: 40

                        onClicked: {
                            Qt.inputMethod.hide();

                            if (searchForWhat === 'address') {
                                rootStack.pop();
                                navitProxy.setLocationPopUp(id);
                                navitProxy.finishSearch();

                            } else {
                                navitProxy.searchSelect(searchForWhat, searchId);
                                var nextSearch;
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

                        }

                        Text {
                            text: itemText
                            color: "white"
                            font.pixelSize: 16
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        NButton {
                            visible: searchForWhat !== 'country' && searchForWhat !== 'address'
                            width: 40
                            height: 40
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            iconSource: "map_icon_white.png"
                            onClicked: {
                                Qt.inputMethod.hide();
                                console.debug('clicked', itemText);
                                rootStack.pop();
                                navitProxy.setLocationPopUp(id);
                                navitProxy.finishSearch();
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: 1
                            anchors.bottom: parent.bottom
                        }
                    }
                }

                ScrollBar {
                    flk: listViewItem
                }
            }
        }
    }

    Connections {
        target: navitProxy
        // @disable-check M16
        onSearchDone: {
            console.debug('search done');
            root.busy = false
            if (searchForWhat === 'country') {
                listViewItem.model = countrySearchResult
            } else if (searchForWhat === 'city') {
                listViewItem.model = citySearchResult
            } else if (searchForWhat === 'street') {
                listViewItem.model = streetSearchResult
            } else if (searchForWhat === 'address') {
                listViewItem.model = addressSearchResult
            }
            resultListViewItem.height = 200
        }
    }
}

