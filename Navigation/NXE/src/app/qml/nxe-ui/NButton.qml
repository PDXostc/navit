import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: root
    width: 100
    height: 100

    signal clicked()

    property string iconSource
    property int iconWidth: 32
    property int iconHeight: 32

    Image {
        source: iconSource
        anchors.centerIn: parent
        sourceSize.width: iconWidth
        sourceSize.height: iconHeight
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
