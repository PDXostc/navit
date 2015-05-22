import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: root
    property int currentOption: 0
    property int maxOptions: options ? options.count : 0

    signal subMenuRequested(string url)
    signal clicked(string menuItem)

    Component.onCompleted: {

        if (type !== 'text') {
            return
        }

        var settingValue = navitProxy.valueFor(settingsValue);
        console.debug('Value for', settingsValue, ' is ', settingValue);

        for(var i=0;i < maxOptions; ++i) {
            if (options.get(i).option === settingValue) {
                currentOption = i;
                break;
            }
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (type === 'text') {
                if (root.currentOption === root.maxOptions - 1) {
                    root.currentOption = 0
                } else {
                    root.currentOption++
                }
                navitProxy.changeValueFor(settingsValue, optionText.text);
            } else if (type === 'sublist') {
                subMenuRequested(options.get(0).url);
            }
            else if (type === 'popup') {
                rootStack.push({item: Qt.resolvedUrl('FactoryResetDialog.qml'), immediate: true});
            }

            else {
                root.clicked(mainTextItem)
            }
        }
    }
    Row {
        anchors.fill: parent

        NText {
            id: mainTextItem
            width: parent.width * 2 / 3
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: itemText
            color: "white"
            font.pixelSize: 18
        }

        Item {
            width: parent.width - mainTextItem.width
            height: parent.height

            NText {
                id: optionText
                text: type === "text" ? options.get(currentOption).option : ""
                visible: type === "text"
                color: "white"
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                font.pixelSize: 16
            }

            Item {
                anchors.fill: parent

                NButton {
                    width: 20
                    height: parent.height
                    visible: type === "sublist"
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    iconSource: "next_icon_white.png"
                    iconWidth: 16
                    iconHeight: 16
                }
            }
        }
    }
    Rectangle {
        width: parent.width
        height: 1
        color: "#45a6c3"
        anchors.bottom: parent.bottom
    }
}
