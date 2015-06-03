import QtQuick 2.0
import "NavigationImages.js" as NI
import '../common'

ListView {
    width: 400
    height: 200

    verticalLayoutDirection: ListView.BottomToTop

    clip: true
    delegate: Item {
        width: parent.width
        height: 60
        Row {
            anchors.fill: parent

            Item {
                id: imageItem
                width: 40
                height: parent.height

                Image {
                    anchors.centerIn: parent
                    source: NI.textToImage(manuverText)
                    sourceSize: Qt.size(32,32)
                }
            }

            Item {
                width: parent.width - imageItem.width
                height: parent.height

                NText {
                    text: manuverText
                    anchors.verticalCenter: parent.verticalCenter
                    wrapMode: Text.WordWrap
                    color: active ? "black" : "grey"
                }
            }
        }
    }
}
