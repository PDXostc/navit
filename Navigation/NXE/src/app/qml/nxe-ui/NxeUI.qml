import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    width: 400
    height: 800

    property bool ftu: navitProxy.ftu

    StackView {
        id: rootStack
        anchors.fill: parent
        initialItem: initialComponent
    }

    Component {
        id: initialComponent

        Loader {
            id: mainPageLoader
            source: ftu ? "FTUMainPage.qml" : "MainPage.qml"

            Connections {
                target: ftu ? mainPageLoader.item : null
                onDownloadMap: {
                    rootStack.push({item:Qt.resolvedUrl("SettingsView.qml"), properties: {ftu: true}})
                }

                onCancelDownload: {
                    rootStack.push(Qt.resolvedUrl("FTUCancelPrompt.qml"))
                }
            }
        }
    }
}
