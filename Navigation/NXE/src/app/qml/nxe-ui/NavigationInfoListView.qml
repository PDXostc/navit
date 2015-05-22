import QtQuick 2.0
import "NavigationImages.js" as NI

ListView {
    width: 400
    height: 200

    clip: true
    delegate: Item {
        width: parent.width
        height: 40
        Row {
            anchors.fill: parent

            Item {
                id: imageItem
                width: 40
                height: parent.height

                Image {
                    anchors.centerIn: parent
                    source: NI.textToImage(manuverText)
                    sourceSize.height: 20
                    sourceSize.width: 20
                }
            }

            Item {
                width: parent.width - imageItem.width
                height: parent.height

                Text {
                    text: manuverText
                    anchors.verticalCenter: parent.verticalCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                    color: active ? "black" : "grey"
                }
            }
        }
    }
}
