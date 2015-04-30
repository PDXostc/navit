import QtQuick 2.0

Item {
    id: locationInfo
    x: 0
    y: 0
    width: 400
    //  width: 400
    height: 100
    visible: true
    signal backToMapRequest()
    property bool isFavorite : false
    property bool canNavigate : false
    Item {
        id: item3
        anchors.fill: parent
        Item {
            id: buttonsBar
            x: 0
            y: 0
            width: 60
            height: parent.height

            Rectangle {
                id: rectangle2
                color: "#242424"
                anchors.fill: parent


                Image {
                    id: image1
                    x: 18
                    y: 30
                    width: 25
                    height: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "back_icon_white_lg.png"
                }

                MouseArea {
                    id: topLocationBarBackButton
                    anchors.fill: parent
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
                anchors.fill: parent

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
}