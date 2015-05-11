import QtQuick 2.0
import QtQuick.Layouts 1.1

Page {
    id: root
    property alias headerText: dialogItem.headerText
    property alias dialogText: dialogItem.dialogText
    property Component leftButton: NButton {
                width: 80
                text: "Cancel"
                onClicked: rootStack.push(Qt.resolvedUrl("FTUCancelPrompt.qml"))
                bold: true
    }

    NDialog {
        id: dialogItem
        iconSource: "download_hex_icon.png"
        bottomBarComponent: Row {
            width: dialogItem.width
            height: 60
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 5

            Loader {
                id: loaderItem
                sourceComponent:  root.leftButton
                height: parent.height
            }

            Item {
                width: parent.width - loaderItem.width - downloadButtonItem.width - 10
                height: parent.height
            }

            NButton {
                id: downloadButtonItem
                text: "Download a map"
                bold: true
                iconSource: "next_icon_white.png"
                onClicked: {
                    rootStack.push({
                                       item: Qt.resolvedUrl("SettingsView.qml"),
                                       properties: {
                                           ftu: true
                                       }
                                   })
                }
            }
        }
    }
}
