import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    width: 400

    color: "white"

    property int defaultHeight: 150
    property ListModel manuverModel: null

    state:'normal'

    Connections {
        target: manuverModel
        onCountChanged: {
            navigationListItem.positionViewAtEnd()
        }
    }

    Behavior on opacity { NumberAnimation {}}
    Behavior on height {NumberAnimation {}}

    Row {
        anchors.fill: parent

        ColumnLayout {
            id: leftBarItem
            width: 50
            height: parent.height
            spacing: 0

            Rectangle {
                width: parent.width
                height: defaultHeight/2
                color: "#242424"

                Image {
                    anchors.centerIn: parent
                    source: "star_icon_empty_grey.png"
                    sourceSize.height: 32
                    sourceSize.width: 32
                }
            }
            Rectangle {
                width: parent.width
                height: defaultHeight/2
                color: "#242424"

                Image {
                    anchors.centerIn: parent
                    source: "star_icon_empty_grey.png"
                    sourceSize.height: 32
                    sourceSize.width: 32
                }
            }

            Item {
               width: parent.width
               height: 10
               Layout.fillHeight: true
            }
        }

        Item {
            height: parent.height
            width: parent.width - leftBarItem.width

            NavigationInfoListView {
                id: navigationListItem
                model: manuverModel
                anchors.fill: parent
                currentIndex: manuverModel.count
                highlightRangeMode: ListView.ApplyRange
                highlightFollowsCurrentItem: true
                anchors.margins: {
                    left: 5
                    right: 5
                    top: 5
                    bottom: 5
                }
            }

             Image {
                width: 20
                height: 20
                source: root.state === 'normal' ? "images/expand_window_icon_grey.png" :
                                                  "images/minimize_window_icon_grey.png"
                anchors {
                    top: parent.top
                    right: parent.right
                    topMargin: 5
                    rightMargin: 5
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.state === 'normal') {
                            root.state = 'expandedManuvers'
                        } else {
                            root.state = 'normal'
                        }
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: root
                height: defaultHeight
            }
        },
        State {
            name: "expandedManuvers"
            PropertyChanges {
                target: root
                height: 400
            }
        }
    ]
}

