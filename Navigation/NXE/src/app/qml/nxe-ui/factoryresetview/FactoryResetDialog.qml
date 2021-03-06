import QtQuick 2.0
import QtQuick.Layouts 1.1
import '../common'

Page {

    NDialog {
        id: dialogItem
        headerText: "Are you sure?"
        width: parent.width
        dialogText: "All saved maps, locations, destinations and settings will be removed."
        bottomBarComponent: Row {
            width: dialogItem.width
            height: 60
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 5

            NButton {
                id: cancelItem
                text: "Cancel"
                iconType: "arrow_right"
                iconHeight: 30
                onClicked: rootStack.pop({
                                             immediate: true
                                         })
            }

            Item {
                width: parent.width - factoryResetItem.width - cancelItem.width - 10
                height: parent.height
            }

            NButton {
                id: factoryResetItem
                text: "Factory Reset Now"
                iconType: "arrow_right"
                iconHeight: 30
                onClicked: {
//                    // we need to reset everything
//                    // set ftu to true
//                    // and pop to the main page
//                    navitProxy.reset();
//                    rootStack.clear();
//                    rootStack.push({
//                                       item: Qt.resolvedUrl("NxeUI.qml")
//                                   })
                    rootStack.push(Qt.resolvedUrl("FactoryResetProgressDialog.qml"));
                }
            }
        }
    }
}
