import QtQuick 2.0

Rectangle {
    id: rectangle2
    width: 380
    height: 850
    color: "#000000"

    property string queryText
    property bool queryBarEnabled: true
    property string customHeader: "Favorites"



    ListView {
        id: locationResultListView
        anchors.topMargin: 0
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        model: locationFavoritesResult
        clip: true
        delegate: LocationsResultDelegate {
            width: parent.width
            height: 50
            onLocationClicked: {
                navitProxy.setLocationPopUp(itemId)
                rootStack.pop();
            }
        }
    }
    ScrollBar {
        flk: locationResultListView
    }
}
