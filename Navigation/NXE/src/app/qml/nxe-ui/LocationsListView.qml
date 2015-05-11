import QtQuick 2.0
import QtQuick.Controls 1.2

ListView {
    id: locationsListView
    signal subMenuRequest(string url)

    delegate: SettingsListDelegate {
        // TODO: Not ready yet
        onSubMenuRequested: subMenuRequest(url)
    }
}
