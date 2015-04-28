import QtQuick 2.0
import QtQuick.Controls 1.2

ListView {
    id: settingListView
    signal subMenuRequest(string url)

    delegate: SettingsListDelegate {
        onSubMenuRequested: subMenuRequest(url)

        onValueChanged: {
            console.debug("Changed " + label + "to " + value);
            if (label === 'Point of Interest') {
                if (value ==='on') {
                    navitProxy.enablePoi = true;
                } else {
                    navitProxy.enablePoi = false;
                }
            }
        }
    }
}
