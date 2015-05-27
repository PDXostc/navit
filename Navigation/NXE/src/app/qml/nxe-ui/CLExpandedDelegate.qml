import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: root
    property bool isDownloaded: false
    signal checked(bool checked)
    property string itemText
    property string itemSize

    function getSizeString(bytes) {
        var suffix = ["B", "kB", "MB", "GB"]
        bytes = parseInt(bytes);
        var res = 0;
        while((bytes = bytes/1024) >= 1)
            res++;
        return ("(" + Math.round(bytes * 1024 * 10) / 10 + " " + suffix[res]+")");
    }

    Item {
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            z:99
            onClicked: {
                checkBoxItem.checked = !checkBoxItem.checked
                root.checked(checkBoxItem.checked)
            }
        }

        NText {
            id: nameText
            color: "#BFBFBF"
            text: mapName
            anchors.left: parent.left
            anchors.leftMargin: 32
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 18

        }

        NText {
            y: 0
            height: parent.height
            color: "#BFBFBF"
            text: getSizeString(itemSize);
            anchors.left: nameText.right
            anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 10
        }


        Rectangle {
            width: parent.width
            height: 1
            color: "#46a5c3"
            anchors.top: parent.top
            anchors.topMargin: 49
        }

        CheckBox {
            id: checkBoxItem
            visible: true
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.verticalCenter: parent.verticalCenter
            enabled: !isDownloaded
            checked: isDownloaded


            style: CheckBoxStyle {
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 2
                    border.color: "white"
                    border.width: 1

                    Rectangle {
                        color: "black"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 1
                        anchors.fill: parent
                        Image {
                            source: "white_tick.png"
                            visible: control.checked
                            sourceSize.width: 16
                            sourceSize.height: 16
                        }
                    }
                }
            }
        }
    }
}
