import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    width: 400
    height: 800

    property bool ftu: false

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
//                    rootStack.push(Qt.resolvedUrl("SettingsView.qml"))
                }
            }

            Connections {
                target: !ftu ? mainPageLoader.item : null
                onPushToStack: {
                    rootStack.push(component)
                }
            }
        }
    }

}
