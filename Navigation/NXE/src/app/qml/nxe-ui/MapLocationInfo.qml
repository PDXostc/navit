import QtQuick 2.0

Item {
    id: locationInfo
    property bool canNavigate: false
    property string locationName: locationComponent ? locationComponent.itemText : ""
    property string locationDescription: locationComponent ? locationComponent.description : ""
    property bool isFavorite: locationComponent ? locationComponent.favorite : false
    property var locationComponent: null
    width: 400
    height: 150

    Connections {
        target: locationComponent ? locationComponent : null
        onFavoriteChanged: {
            console.debug('fav changed ', locationComponent.favorite)
        }
    }

    Item {
        id: item3
        width: parent.width
        height: 150
        anchors.bottom: parent.bottom
        Item {
            id: buttonsBar
            x: 0
            y: 0
            width: 60
            height: parent.height

            Rectangle {
                id: rectangle2
                anchors.fill: parent
                color: "#242424"

                MouseArea {
                    id: mouseArea1
                    x: 0
                    y: 0
                    width: 60
                    height: 75
                    onClicked: {
                        if(locationComponent) {
                            console.debug(locationComponent.favorite)
                            locationComponent.favorite = !locationComponent.favorite
                        }
                    }

                    Image {
                        id: image1
                        x: 12
                        y: 19
                        width: 28
                        height: 28
                        visible: true
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenterOffset: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: isFavorite ? "star_icon_solid_white.png" : "star_icon_empty_grey.png"
                    }
                }

                MouseArea {
                    id: mouseArea2
                    x: 0
                    y: 75
                    width: 60
                    height: 75

                    onClicked: {
                        startNavigationButton.opacity = 1
                    }

                    Image {
                        id: image2
                        x: 12
                        y: 39
                        width: 28
                        height: 28
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenterOffset: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "navigate_icon_white.png"
                    }
                }
            }
        }

        Item {
            id: item1
            x: 58
            y: 0
            anchors.left: buttonsBar.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.leftMargin: 0

            Rectangle {
                id: rectangle1
                color: "#fbfbfb"
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent

                MouseArea {
                    id: startNavigationButton
                    x: 43
                    width: 289
                    height: 36
                    anchors.top: locationDescription.bottom
                    anchors.topMargin: 10
                    opacity: 0

                    Behavior on opacity { NumberAnimation {} }

                    Image {
                        anchors.fill: parent
                        source: "blue_forward_button_long_bg.png"
                    }

                    Text {
                        x: 8
                        y: 8
                        color: "#ffffff"
                        text: qsTr("Start Navigation")
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 15
                    }
                }

                Image {
                    x: 58
                    y: 21
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    source: "location_marker_icon.png"
                }

                Text {
                    id: locationTitleTextItem
                    x: 43
                    y: 21
                    text: locationName
                    font.pixelSize: 26
                }

                Text {
                    x: 43
                    text: locationInfo.locationDescription
                    anchors.top: locationTitleTextItem.bottom
                    anchors.topMargin: 5
                    font.pixelSize: 15
                }

                Text {
                    x: 302
                    y: 36
                    color: "#09bcdf"
                    text: qsTr("3 min")
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: 12
                }

                Text {
                    x: 284
                    y: 58
                    color: "#09bcdf"
                    text: qsTr("0.2 miles")
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: 12
                }
            }
        }
    }
}
