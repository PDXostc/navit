import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    width: 120
    height: 120

    signal clicked()

    Rectangle {
        id: menuBackground
        color: "black"
        anchors.fill: parent
        opacity: theme.menu.opacity
    }

    RectangularGlow {
        anchors.fill: menuBackground
        glowRadius: 8
        spread: 0.2
        color: "#66000000"
    }

    NText {
        anchors.fill: parent
        text: "\uf104"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 60
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
