import QtQuick 2.0

QtObject {
    property string pageBackgroundColor: "black"

    property QtObject menu:  QtObject {
        property string backgroundColor: "black"
        property int buttonSize: 40
        property real opacity: 0.85
    }

    property QtObject page: QtObject{
        property string bck: "black"
    }

    property QtObject settingsHeaderView: QtObject {
        property int height: 80
    }

    property QtObject settingsView: QtObject {
        property int delHeight: 70
    }

    property QtObject ssearchView: QtObject {
        property int titleHeight: 60
        property int startOverHeight: 60
        property int resultHeight: 70
        property int resultFontSize: 28
        property int selectedHeight: 100
        property string selectedBck: "#09bcdf"
    }

    property QtObject dialog: QtObject {
        property int buttonSize: 96
        property int height: 450
        property int bottomBarHeight: 120
    }


    property int defFontSize: 30
    property int bigFontSize: 40
    property int defButtonSize: 48
    property string componentBck: "#09bcdf"
}
