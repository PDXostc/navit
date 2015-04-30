import QtQuick 2.0

Item {
    id: locationInfo
    x: 0
    y: 0
    width: 400
    //  width: 400
    height: 205
    visible: true
    signal backToMapRequest()
    property bool isFavorite : false
    property bool canNavigate : false
    Item {
        id: item3
        x: 0
        y: 0
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
                   // onClicked: isFavorite = isFavorite ? false : true
                    onClicked: isFavorite = !isFavorite
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

    Image {
        id: blueBarImage
        anchors.fill: parent
        source: "blue_forward_button_long_bg.png"
    }

    Text {
        id: text1
        x: 8
        y: 8
        color: "#ffffff"
        text: qsTr("Start Navigation")
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 15
    }
}

Image {
    id: image3
                    x: 58
                    y: 21
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    source: "location_marker_icon.png"
                }

                Text {
                    id: locationTitle
                    x: 43
                    y: 21
                    text: qsTr("Location Name")
                    font.pixelSize: 26
                }

                Text {
                    id: locationDescription
                    x: 43
                    text: qsTr("1234 N Main, Portland, OR 97208")
                    anchors.top: locationTitle.bottom
                    anchors.topMargin: 5
                    font.pixelSize: 15
                }

                Text {
                    id: locationTime
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
                    id: locationDistance
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

    MapLocationToast {
        id: mapLocationToast1
        x: 0
        y: 0
        width: parent.width
        anchors.bottom: item3.top
        anchors.bottomMargin: 5
    }
}
