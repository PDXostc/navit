import QtQuick 2.0
import QtQuick.Controls 1.2

ListView {
    id: settingsListView
    signal subMenuRequest(string url)

    delegate: SettingsListDelegate {
        onSubMenuRequested: subMenuRequest(url)
    }
}
