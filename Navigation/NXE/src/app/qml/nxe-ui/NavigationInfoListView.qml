import QtQuick 2.0

ListView {
    width: 400
    height: 200

    function type(text) {
        var turnLeftRegexp = new RegExp("Turn.*left.*")
        var turnRightRegexp = new RegExp("Turn.*right.*")
        var followRegexp = new RegExp("Follow the road.*")
        var turnAroundRegexp = new RegExp(".*turn around.*")

        var turnLeft2Regexp = new RegExp("Take the second.*left.*");
        var turnRight2Regexp = new RegExp("Take the second.*right.*");

        if(text.match(turnLeftRegexp)) {
            return turnLeft;
        } else if(text.match(turnRightRegexp)) {
            return turnRight;
        } else if(text.match(followRegexp)) {
            return keepStraight;
        } else if(text.match(turnAroundRegexp)) {
            return turnAround;
        } else if(text.match(turnLeft2Regexp)) {
            return turnLeft2;
        } else if(text.match(turnRight2Regexp)) {
            return turnRight2;
        } else {
            console.debug('oups what to do with ', text)
        }
    }

    property var manuverImages: ["images/nav_left_1_bk.png",
                                 "images/nav_right_1_bk.png",
                                 "images/nav_straight_bk.png",
                                 "images/nav_turnaround_left_bk.png",
                                 "images/nav_left_2_bk.png",
                                 "images/nav_right_2_bk.png",
                                ]

    property int turnLeft: 0
    property int turnRight: 1
    property int keepStraight: 2
    property int turnAround: 3
    property int turnLeft2: 4
    property int turnRight2: 5

    clip: true
    delegate: Item {
        width: parent.width
        height: 40
        Row {
            anchors.fill: parent

            Item {
                id: imageItem
                width: 40
                height: parent.height

                Image {
                    anchors.centerIn: parent
                    source: manuverImages[type(manuverText)]
                    sourceSize.height: 20
                    sourceSize.width: 20
                }
            }

            Item {
                width: parent.width - imageItem.width
                height: parent.height

                Text {
                    text: manuverText
                    anchors.verticalCenter: parent.verticalCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                    color: active ? "black" : "grey"
                }
            }
        }
    }
}
