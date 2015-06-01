import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

Item {
    id: root
    property bool canNavigate: false
    property string locationName: locationComponent ? locationComponent.itemText : ""
    property string locationDescription: locationComponent ? locationComponent.description : ""
    property int locationDistance: locationComponent ? locationComponent.distance : -1
    property int locationEta: locationComponent ? locationComponent.eta : -1
    property bool isFavorite: locationComponent ? locationComponent.favorite : false
    property var locationComponent: null
    width: 600
    height: 350

    state: 'normal'

    signal navigationUIRequested()

    function etaText() {
        var str;
        console.debug('eta ', locationComponent.eta)
        if (locationEta === -1) {
            str = qsTr("Unknown eta");
        } else {
            if (locationComponent.eta < 60) {
                str = locationComponent.eta + " sec";
            } else {
                var minutes = Math.floor(locationComponent.eta / 60);
                str = minutes + " min";
            }
        }
        console.debug(str)
        return str;
    }

    function distanceText() {
        var destStr;
        var dest = locationComponent.distance
        if (dest === -1) {
            destStr= "N/A";
        }

        if (dest > 1000) {
            var dist = Number(dest / 1000).toFixed(1)
            destStr = dist + " km"
        } else {
            destStr = dest + " m"
        }
        console.debug('distance=', destStr, dest)
        return destStr
    }


    function toggleNaviButton() {
        if (root.state === 'normal') {
            root.state = 'navi';
            navigationProxy.startNavigation(navitProxy.currentlySelectedItem);
        } else {
            root.state = 'normal'
            navigationProxy.stopNavigation();
        }
    }

    Connections {
        target: locationComponent ? locationComponent : null
        onFavoriteChanged: {
            console.debug('fav changed ', locationComponent.favorite)
        }

        onEtaChanged: {
            locationEtaTextItem.text = etaText();
        }

        onDistanceChanged: {
            locationDistanceTextItem.text = distanceText();
        }
    }

    Behavior on opacity {NumberAnimation {} }

    Rectangle {
        anchors.fill: parent

        Item {
            id: buttonsBar
            width: 120
            height: parent.height

            Rectangle {
                anchors.fill: parent
                color: "#242424"

                Column {
                    anchors.fill: parent

                    MouseArea {
                        width: parent.width
                        height: parent.height/2
                        onClicked: {
                            if (locationComponent) {
                                console.debug(locationComponent.favorite)
                                locationComponent.favorite = !locationComponent.favorite
                            }
                        }
                        Image {
                            sourceSize: theme.bigIconSize
                            anchors.centerIn: parent
                            source: isFavorite ? "star_icon_solid_white.png" : "star_icon_empty_grey.png"
                        }
                    }

                    MouseArea {
                        width: parent.width
                        height: parent.height/2

                        onClicked: {
                            toggleNaviButton()
                        }

                        Image {
                            sourceSize: theme.bigIconSize
                            anchors.centerIn: parent
                            source: "navigate_icon_white.png"
                        }
                    }
                }
            }
        }

        Item {
            anchors{
                left: buttonsBar.right
                right: parent.right
                bottom: parent.bottom
                top: parent.top
            }

            Column {
                anchors.fill: parent

                Column {
                    width: parent.width
                    height: parent.height/2

                    RowLayout {
                        anchors {
                            left: parent.left
                            right: parent.right
                            leftMargin: 5
                            rightMargin: 20
                        }
                        height: parent.height * 2/3
                        spacing: 15

                        Item {
                            width: 60
                            height: parent.height

                            Image {
                                id: locationImage
                                sourceSize: theme.bigIconSize
                                anchors.verticalCenter: parent.verticalCenter
                                source: "location_marker_icon.png"
                                smooth: true
                            }

                            NDropShadow {
                                anchors.fill: locationImage
                                source: locationImage
                            }
                        }

                        NText {
                            text: locationName
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "black"
                        }

                        NText {
                            id: locationEtaTextItem
                            text: "3 min"
                            color: "#09bcdf"
                            font.pixelSize: 20
                        }

                    }

                    RowLayout {
                        anchors {
                            left: parent.left
                            right: parent.right
                            leftMargin: 80
                            rightMargin: 20
                        }
                        height: parent.height * 1/3

                        NText {
                            text: locationDescription
                            verticalAlignment: Text.AlignTop
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "black"
                            font.pixelSize: 24
                        }

                        NText {
                            id: locationDistanceTextItem
                            text: "0.2 miles"
                            Layout.fillHeight: true
                            verticalAlignment: Text.AlignTop
                            color: "#09bcdf"
                            font.pixelSize: 20
                        }
                    }
                }

                Item {
                    height: parent.height/2
                    anchors {
                        left: parent.left
                        leftMargin: 80
                        right: parent.right
                        rightMargin: 20
                    }


                    Item {
                        id: startNavigationItem
                        anchors.fill: parent
                        Behavior on opacity {NumberAnimation {}}

                        MouseArea {
                            anchors.fill: parent
                            onClicked: navigationUIRequested()

                            Image {
                                source: "blue_forward_button_long_bg.png"
                                width: parent.width
                                anchors.verticalCenter: parent.verticalCenter

                                NText {
                                    text: "x"
                                    anchors.right: parent.right
                                    anchors.rightMargin: 40
                                    anchors.verticalCenter: parent.verticalCenter
                                    font.pixelSize: 18
                                }
                            }

                            NText {
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                text: qsTr("Start Navigation")
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: startNavigationItem
                opacity:0
            }
        },
        State {
            name: "navi"
            PropertyChanges {
                target: startNavigationItem
                opacity:1
            }
        }
    ]
}
