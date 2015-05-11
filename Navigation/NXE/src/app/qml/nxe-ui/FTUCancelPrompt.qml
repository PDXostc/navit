import QtQuick 2.0

FTUDialogPage {
    dialogText: "Navit can't run without a map"
    headerText: "Are you sure"

    leftButton: NButton {
        width: 80
        text: "Quit"
        onClicked: navitProxy.quit()
        bold: true
    }
}
