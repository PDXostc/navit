import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    width: 300
    height: 300

    signal clicked()

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.clicked()
            }
        }
    }

    Image {
        id: butterfly
        source: "images/cancel_banner_icon.png"
        sourceSize: Qt.size(parent.width, parent.height)
        smooth: true
        visible: false
    }

    DropShadow {
        anchors.fill: butterfly
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8.0
        transparentBorder: true
        samples: 16
        color: "#80000000"
        source: butterfly
    }
}
