import QtQuick 2.0

Item {
    id: locationInfo
    x: 0
    y: 0
    width: 400
    height: 150
    visible: true
    signal backToMapRequest()

    Item {
        id: item3
        x: 0
        y: 0
        width: 400
        height: 150
        anchors.bottom: parent.bottom
        Item {
            id: item1
            x: 58
            y: 0
            anchors.fill: parent

            Rectangle {
                id: rectangle1
                color: "#000000"
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
            }
        }
        Item {
            id: item2
            x: 0
            y: 0
            width: 60
            height: parent.height

            Rectangle {
                id: rectangle2
                height: 150
                anchors.fill: parent
                color: "#242424"

                MouseArea {
                    id: mouseArea1
                    width: 60
                    height: 75

                    Image {
                        id: image1
                        x: 12
                        y: 19
                        width: 30
                        height: 30
                        visible: false
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenterOffset: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "star_icon_solid_white.png"
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
                        width: 30
                        height: 30
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenterOffset: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "navigate_icon_white.png"
                    }
                }
            }
        }

    }
}
