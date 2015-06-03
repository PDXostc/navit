import QtQuick 2.0
import QtQuick.Layouts 1.1

Item {
    id: root
    width: 400

    property int defaultHeight: 320
    property ListModel manuverModel: null
    property var naviComponent: null

    state:'normal'

    Connections {
        target: manuverModel
        onCountChanged: {
            navigationListItem.positionViewAtEnd()
        }
    }

    Behavior on opacity { NumberAnimation {}}
    Behavior on height {NumberAnimation {}}

    ColumnLayout{
        anchors.fill: parent

        NavigationInfoExtraInfo {
            id: infoItem
            Layout.fillWidth: true
            height: 120
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight:true

            Row {
                anchors.fill: parent

                ColumnLayout {
                    id: leftBarItem
                    width: 120
                    height: parent.height
                    spacing: 0

                    Rectangle {
                        width: parent.width
                        height: 100
                        color: "#242424"

                        MouseArea {
                            anchors.fill: parent
                        }

                        Image {
                            anchors.centerIn: parent
                            source: "../images/navigate_icon_white.png"
                            sourceSize: theme.bigIconSize
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 100
                        color: "#242424"

                        Image {
                            anchors.centerIn: parent
                            source: naviComponent.favorite ? "../images/star_icon_solid_white.png" : "../images/star_icon_empty_grey.png"
                            sourceSize: theme.bigIconSize
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                naviComponent.favorite = !naviComponent.favorite;
                            }
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
                        width: 40
                        height: 40
                        source: root.state === 'normal' ? "../images/expand_window_icon_grey.png" :
                                                          "../images/minimize_window_icon_grey.png"
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
                height: defaultHeight + 200
            }
        }
    ]
}

