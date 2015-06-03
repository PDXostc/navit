import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQml 2.2
import '../common'

Item {
    id: root
    width: 400
    height: 300
    focus: true
    property alias model: listViewItem.model
    property int numberOfCharsToStartSearch: 2
    property string lastSearchedString

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
            var i = model.length
            console.debug('list size=',i)
            var size = i * (theme.ssearchView.resultHeight)
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
        focus: true
        spacing: 5
        anchors {
            leftMargin: 5
            rightMargin: 5
        }

        Rectangle {
            id: searchInputItem
            height: theme.defFontSize + 10
            width: parent.width
            color: "white"
            focus: true

            TextEdit {
                id: searchInputTextItem
                color: "#242424"
                font.pixelSize: theme.defFontSize
                anchors.fill: parent
                anchors.leftMargin: 5
                Component.onCompleted: {
                    searchInputTextItem.focus = true
                    searchInputTextItem.forceActiveFocus();
                }
                onTextChanged:  {
                    if(text === lastSearchedString) {
                        console.debug('Last search string is ', lastSearchedString, ' we will not search for it')
                        return
                    }
                    if (text.length >= numberOfCharsToStartSearch) {
                        lastSearchedString = text
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
                    height: theme.ssearchView.resultHeight

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

                            NText {
                                width: parent.width
                                height: parent.height / 2
                                text: itemText
                                font.pixelSize: theme.ssearchView.resultFontSize
                            }
                            NText {
                                width: parent.width
                                height: parent.height / 2
                                text: itemText
                                font.pixelSize: theme.ssearchView.resultFontSize - 6
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
