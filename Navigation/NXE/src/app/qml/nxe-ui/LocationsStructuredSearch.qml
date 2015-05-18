import QtQuick 2.0
import QtQml 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Page {
    id: root

    property string selectedItemText
    property string selectedItemDescription
    property var selectedItemId
    property bool searchInProgress: false

    // search state machine
    property int __searchStateId: __CountrySearch
    property int __CountrySearch: 0
    property int __CitySearch: 1
    property int __StreetSearch: 2
    property int __AddressSearch: 3
    property int __FinishSearch: 4

    property var __searchFunctions: [function (str) {
        navitProxy.searchCountry(str)
    }, function (str) {
        navitProxy.searchCity(str)
    }, function (str) {
        navitProxy.searchStreet(str)
    }, function (str) {
        navitProxy.searchAddress(str)
    }]

    property var __searchModels: [countrySearchResult, citySearchResult, streetSearchResult, addressSearchResult]
    property var __searchHeaders: ["Country", "City", "Add a Street", "Add an Address Number"]
    property var __searchSelectTag: ['country', 'city', 'street','address'];

    // Hack for weekeyboard
    property string currentSearchString

    function startPredictiveSearch(string) {
        if (root.busy)
            return
        listViewItem.model = null
        root.busy = true
        currentSearchString = string
        console.debug('start searching for ', currentSearchString)
        __searchFunctions[__searchStateId](string)
    }

    Component.onCompleted: {
        console.debug(selectedItemText)
        if (__searchStateId === __CountrySearch) {
            navitProxy.startSearch()
        } else if (__searchStateId === __AddressSearch) {
            console.debug('start predictive search before')
            // Search street address even if no input is provided
            startPredictiveSearch('')
        }
    }
    ColumnLayout {
        anchors {
            fill: parent
            leftMargin: 10
            rightMargin: 10
        }

        LocationsStructuredSearchSelected {
            id: selectedItem
            title: selectedItemText
            description: selectedItemDescription
            width: parent.width
            height: selectedItemText.length === 0 ? 0 : 50
            Layout.fillWidth: true

            onCarButtonClicked: {
                Qt.inputMethod.hide()
                rootStack.pop()
                navitProxy.setLocationPopUp(selectedItemId)
                navitProxy.finishSearch()
            }
        }

        Item {
            width: parent.width
            height: selectedItem.height === 0 ? 0 : 10
        }

        Item {
            id: titleItem
            visible: __searchStateId !== __FinishSearch
            width: parent.width
            height: 30

            Text {
                text: __searchStateId !== __FinishSearch ? __searchHeaders[__searchStateId] : ""
                color: 'white'
                font.pixelSize: 16
                anchors.fill: parent
                anchors.leftMargin: 5
            }
        }

        LocationsSearchInputItem {
            id: listViewItem
            width: parent.width
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: __searchStateId !== __FinishSearch
            onTriggerSearch: {
                console.debug('text has changed to', searchText, ' start searching')
                startPredictiveSearch(searchText);
            }

            onItemSelected: {
                navitProxy.searchSelect(__searchSelectTag[__searchStateId], searchId)
                var nextSearch = __searchStateId + 1

                searchStackView.push({
                                         item: Qt.resolvedUrl(
                                                   "LocationsStructuredSearch.qml"),
                                         properties: {
                                             __searchStateId: nextSearch,
                                             selectedItemText: itemText,
                                             selectedItemDescription: itemDescription,
                                             selectedItemId: itemId
                                         }
                                     })
            }
        }
    }

    Connections {
        target: navitProxy
        // @disable-check M16
        onSearchDone: {
            console.debug('search is done')
            root.busy = false
            console.debug(__searchStateId, __searchModels[__searchStateId], __searchModels[__searchStateId])
            listViewItem.model = __searchModels[__searchStateId]
        }
    }
}
