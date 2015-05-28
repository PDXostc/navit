import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import 'themes'

Item {
    width: 400
    height: 800

    property var theme: themeLoader.item
    property bool ftu: navitProxy.ftu

    FontLoader {
        source: "fonts/SourceSansPro-Regular.ttf"
    }

    FontLoader {
        source: "fonts/fontawesome-webfont.ttf"
    }

    StackView {
        id: rootStack
        anchors.fill: parent
        initialItem: initialComponent
    }

    Loader {
        id: themeLoader
        sourceComponent: TizenTheme {
            id: theme
        }
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
