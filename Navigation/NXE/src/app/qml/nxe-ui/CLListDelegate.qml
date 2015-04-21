import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: root
    width: 400
    height: 50

    signal checked()

    SettingsListDelegate {
        width: parent.width
        height: parent.height
    }

    CheckBox {
        visible: type === 'empty'
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        onClicked: {
            root.checked()
        }

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
                        source: "search_icon_white_sm.png"
                        visible: control.checked
                        sourceSize.width: 16
                        sourceSize.height: 16
                    }
                }
            }
        }
    }
}
