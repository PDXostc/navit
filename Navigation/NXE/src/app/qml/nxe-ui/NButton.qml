import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

Item {
    id: root
    property bool bold
    property bool _iconVisible: iconSource !== ''
    property alias text: buttonText.text
    property alias iconSource: imageItem.source
    property bool iconVisible: iconSource.toString().length !== 0
    property alias iconWidth: imageItem.sourceSize.width
    property alias iconHeight: imageItem.sourceSize.height
    property alias layout: rowLayout.layoutDirection

    height: 60

    function recalculateWidth() {
        var w = buttonText.contentWidth + 5
        if (iconVisible)
            w += imageItem.width

        root.width = w;
    }

    signal clicked

    Component.onCompleted: {
        recalculateWidth()
    }

    Rectangle {
        visible: false
        anchors.fill: parent
        color: "green"
    }

    Item {
        anchors.fill: parent

        Row {
            id: rowLayout
            anchors.fill: parent
            spacing: 5

            NText {
                id: buttonText
                font.bold: root.bold
                font.pixelSize: 18
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                height: parent.height
                onTextChanged: {
                    recalculateWidth()
                }
            }

            Image {
                id: imageItem
                visible: iconVisible
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.clicked()
        }
    }
}
