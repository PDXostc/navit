import QtQuick 2.0

Item {
    id: root
    width: 60
    height: 4 * 60
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: parent.width * 0.1

    signal clicked(string item)

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.85
    }

    Column {
        anchors.fill: parent

        Item {
            width: parent.width
            height: width

            NButton {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.fill: parent
                iconSource: "menu_icon_white.png"
                onClicked: root.clicked("menu")
            }
        }

        Item {
            width: parent.width
            height: width

            NButton {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.fill: parent
                iconSource: "search_icon_white_sm.png"
            }
        }
        Item {
            width: 60
            height: width

            NButton {
                x: 0
                anchors.rightMargin: 5
                anchors.leftMargin: 5
                anchors.bottomMargin: 5
                anchors.topMargin: 5
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.fill: parent
                iconSource: "northup_icon_white.png"
                onClicked: root.clicked('orientation')
            }
        }
        Item {
            width: parent.width
            height: width

            NButton {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.fill: parent
                iconSource: "car_icon_white.png"
            }
        }
    }
}
