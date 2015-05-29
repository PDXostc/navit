import QtQuick 2.0

QtObject {
    property string pageBackgroundColor: "black"

    property QtObject menu:  QtObject {
        property string backgroundColor: "black"
        property int buttonSize: 60
        property real opacity: 0.85
        property QtObject size: QtObject {
            property int width: 120
            property int height: 480
        }

        property int topMargin: 140
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

    property QtObject locationDetails: QtObject {
        property int buttonSize: 48
    }


    property int defFontSize: 30
    property int bigFontSize: 40
    property int defButtonSize: 48
    property size bigIconSize: Qt.size(60,60)
    property string componentBck: "#09bcdf"
}
