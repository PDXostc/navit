import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    width: 400
    height: 800

    property bool ftu: navitProxy.ftu

    FontLoader {
        source: "fonts/SourceSansPro-Regular.ttf"
        onNameChanged: console.debug(name)
    }

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
                target: navitProxy
                onFtuChanged: {
                    console.debug('ftu changed!', ftu)
                    if (ftu) {
                        mainPageLoader.source = Qt.resolvedUrl("FTUMainPage.qml")
                    }
                }
            }
        }
    }
}
