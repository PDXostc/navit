import QtQuick 2.0
import QtQuick.Layouts 1.1
import '../common'

Rectangle {
    width: 400
    height: 320
    property var locationComponent: null
    property bool isFavorite: locationComponent ? locationComponent.favorite : false
    property string locationName: locationComponent ? locationComponent.itemText : ""
    property string locationDescription: locationComponent ? locationComponent.description : ""

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            height: 80

            RowLayout {
                anchors.fill: parent
                Rectangle {
                    Layout.fillHeight: true
                    width: 120
                    color: "#242424"
                    Image {
                        anchors.centerIn: parent
                        source: isFavorite ? "../images/star_icon_solid_white.png" : "../images/star_icon_empty_grey.png"
                        sourceSize: theme.bigIconSize
                    }
                }

                Item {
                    Layout.fillHeight: true
                    width: 40
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Column {
                        anchors.fill: parent
                        Item {
                            width: parent.width
                            height: parent.height/2

                            NText {
                                color: "black"
                                text: locationName
                                font.pixelSize: 26
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        Item {
                            width: parent.width
                            height: parent.height/2
                            NText {
                                color: "black"
                                text: locationDescription
                                font.pixelSize: 20
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }
            }
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Column {
                anchors.fill: parent
                anchors.rightMargin: 5

                Item {
                    width: parent.width
                    height: parent.height/2
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            navigationProxy.startNavigation(locationComponent);
                            navitProxy.clearWaypoint()
                        }
                    }
                    Rectangle {
                        width: 120
                        height: parent.height
                        color: "#242424"
                    }
                    Image {
                        width: parent.width
                        height: parent.height - 20
                        source: "../images/blue_forward_button_long_bg.png"
                        sourceSize.height: 40
                        anchors.verticalCenter: parent.verticalCenter

                        Row {
                            anchors {
                                fill: parent
                                leftMargin: 5
                            }
                            spacing: 25
                            Image {
                                source: "../images/navigate_icon_white.png"
                                sourceSize.width: 32
                                sourceSize.height: 32
                                anchors.verticalCenter:parent.verticalCenter
                            }

                            NText {
                                text: "Navigate to this location"
                                anchors.verticalCenter:parent.verticalCenter
                                font.pixelSize: 20
                            }
                        }
                    }
                }
                Item {
                    width: parent.width
                    height: parent.height/2
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            navigationProxy.addWaypoint(locationComponent)
                            navitProxy.clearWaypoint()
                        }
                    }
                    Rectangle {
                        width: 120
                        height: parent.height
                        color: "#242424"
                    }
                    Image {
                        width: parent.width
                        height: parent.height - 20
                        source: "../images/blue_forward_button_long_bg.png"
                        sourceSize.height: 40
                        anchors.verticalCenter: parent.verticalCenter
                        Row {
                            anchors {
                                fill: parent
                                leftMargin: 5
                            }
                            spacing: 25
                            Image {
                                source: "../images/navigate_icon_white.png"
                                sourceSize.width: 32
                                sourceSize.height: 32
                                anchors.verticalCenter:parent.verticalCenter
                            }

                            NText {
                                text: "Add waypoint to current route"
                                anchors.verticalCenter:parent.verticalCenter
                                font.pixelSize: 20
                            }
                        }
                    }
                }
            }
        }
    }
}

