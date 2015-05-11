import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: root
    width: 400
    height: 800
    color: "black"

    signal backToMapRequest
    property bool ftu: false

    Column {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 10

        spacing: 10

        SettingsViewHeader {
            id: settingsHeaderItem
            width: parent.width
            height: 50
            stack: settingsStackView
            header: "Settings"
        }

        StackView {
            id: settingsStackView
            Component.onCompleted: {
                if (ftu) {
                    settingsStackView.push(Qt.resolvedUrl("CountryLanguage.qml"))
                    settingsHeaderItem.isHeaderEnabled = false;
                }
            }

            onCurrentItemChanged: {
                if (currentItem && currentItem.customHeader) {
                    console.debug(currentItem, currentItem.customHeader)
                    settingsHeaderItem.header = currentItem.customHeader
                }
            }


            initialItem: Item {
                property string customHeader: "Settings"
                width: parent.width
                height: parent.height - 100

                SettingsListView {
                    id: settingsListView
                    model: SettingsListModel {}
                    anchors.fill: parent
                    clip: true
                    onSubMenuRequest: settingsStackView.push(
                                          Qt.resolvedUrl(url))
                }
                ScrollBar {
                    flk: settingsListView
                }
            }
        }
    }
}
