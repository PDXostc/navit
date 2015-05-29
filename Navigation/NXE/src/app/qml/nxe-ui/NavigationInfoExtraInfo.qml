import QtQuick 2.0
import QtQuick.Layouts 1.1

Item {
    id: root
    width: 400
    height: 60

    state: 'normal'

    RowLayout {
        id: mainItem
        width: 120
        height: parent.height
        spacing: 0

        Behavior on width {NumberAnimation {}}

        Item {
            width: 120
            Layout.fillHeight: true

            Item {
                height: parent.height
                width: 120

                Rectangle {
                    anchors.fill: parent
                    color: "black"
                    opacity: 0.8
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (root.state === 'normal') {
                                root.state = 'expanded'
                            } else {
                                root.state = 'normal'
                            }
                        }
                    }
                }

                Image {
                    source: "images/info_icon_white.png"
                    sourceSize: theme.bigIconSize
                    anchors.centerIn: parent

                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 0.8

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        Layout.fillWidth: true
                        height: 30

                        NText {
                            opacity: root.state === 'normal' ? 0 : 1
                            Behavior on opacity {NumberAnimation {}}
                            text: "Current:  " + navigationProxy.currentStreet
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                        height: 30

                        NText {
                            opacity: root.state === 'normal' ? 0 : 1
                            Behavior on opacity {NumberAnimation {}}
                            text: "Next:  No data available"
                            anchors.verticalCenter: parent.verticalCenter
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
                target: mainItem
                width: 70
            }
        },
        State {
            name: "expanded"
            PropertyChanges {
                target: mainItem
                width: root.width
            }
        }
    ]
}
