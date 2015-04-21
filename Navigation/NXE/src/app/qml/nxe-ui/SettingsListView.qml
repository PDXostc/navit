import QtQuick 2.0
import QtQuick.Controls 1.2

ListView {
    id: settingListView
    signal subMenuRequest(string url)

    delegate: SettingsListDelegate {
        onSubMenuRequested: subMenuRequest(url)
    }
}
