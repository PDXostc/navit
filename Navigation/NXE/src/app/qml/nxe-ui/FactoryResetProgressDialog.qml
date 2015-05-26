import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Page {
    Component.onCompleted: {
        navitProxy.reset();
        navitProxy.stopNavit();
    }

    NDialog {
        id: dialogItem
        headerText: "Resetting Navit..."
        width: parent.width
        dialogText: ""
        iconSource: ""
        bckColor: "#4795a9"

        ProgressBar {
            id: progressBarItem
            value: 0

            onValueChanged: {
                if (value >= 1) {
                    fakeBusyTimer.stop();
                    rootStack.clear();
                    rootStack.push({
                                       item: Qt.resolvedUrl("NxeUI.qml")
                                   })
                }
            }
            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            height: 15
            Behavior on value {
                NumberAnimation {
                }
            }
            style: ProgressBarStyle {
                background: Rectangle {
                    radius: 2
                    color: "#467C8A"
                    border.color: "#467C8A"
                    border.width: 1
                    implicitWidth: 200
                    implicitHeight: 8
                }
                progress: Rectangle {
                    color: "#BABABA"
                    border.color: "#BABABA"
                }
            }
        }
    }

    Timer {
        id: fakeBusyTimer
        running: true
        interval: 500
        repeat: true
        onTriggered: {
            progressBarItem.value += 0.08
        }
    }
}

