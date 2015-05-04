import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: root
    width: 400
    height: 50
    property bool downloaded: false

    signal checked()

    signal subList(string url)

    SettingsListDelegate {
        width: parent.width
        height: parent.height

        onSubMenuRequested: {
            console.debug('Clicked' + url)
            root.subList(url)
        }
    }

    CheckBox {
        visible: type === 'empty'
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        enabled: !downloaded

        checked: downloaded

        onClicked: {
            root.checked(checked)
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
