import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQml 2.2

Item {
    id: root
    width: 400
    height: 300
    property alias model: listViewItem.model
    property int numberOfCharsToStartSearch: 2

    signal itemSelected(var itemId, var searchId, string itemText, string itemDescription)
    signal triggerSearch(string searchText)

    onStateChanged: {
        console.debug('state changed')
        resultListViewItem.height = calculateListSize()
    }

    onModelChanged: {
        if(model) {
            console.debug('Model changed = ', model, model.length)
            resultListViewItem.height = calculateListSize()
        }
    }

    function calculateListSize() {
        if (model) {
            var size = model.length * 40
            console.debug(model, model.length, size, root.height)
            if (size > (root.height - 100)) {
                size = root.height - 100
            }
            return size
        } else {
            return 0
        }
    }

    Column {
        anchors.fill: parent
        spacing: 5
        anchors {
            leftMargin: 5
            rightMargin: 5
        }

        Rectangle {
            id: searchInputItem
            height: 20
            width: parent.width
            color: "white"

            TextEdit {
                id: searchInputTextItem
                color: "#242424"
                font.pixelSize: 15
                anchors.fill: parent
                anchors.leftMargin: 5
                onTextChanged:  {
                    if (text.length >= numberOfCharsToStartSearch) {
                        triggerSearch(text)
                    }
                }
            }
        }

        Item {
            id: resultListViewItem
            width: searchInputTextItem.width
            height: 0

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "white"
            }

            Behavior on height {
                NumberAnimation {
                }
            }

            Rectangle {
                anchors.fill: parent
                color: "#09bcdf"
            }
            ListView {
                id: listViewItem
                anchors.fill: parent
                clip: true

                delegate: MouseArea {
                    width: resultListViewItem.width
                    height: 40

                    onClicked: {
                        Qt.inputMethod.hide()
                        itemSelected(itemId, searchId, itemText, description)
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: "#09bcdf"

                        Column {
                            anchors.fill: parent
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5

                            Text {
                                width: parent.width
                                color: "white"
                                font.pixelSize: 16
                                height: parent.height / 2
                                text: itemText
                            }
                            Text {
                                width: parent.width
                                color: "white"
                                font.pixelSize: 12
                                height: parent.height / 2
                                text: itemText
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        anchors.bottom: parent.bottom
                    }
                }
            }

            ScrollBar {
                flk: listViewItem
            }
        }
    }
}
