import QtQuick 2.0

QtObject {
    id: navigationProxy
    property int distanceToDestination: 5445
    property bool navigation: false
    property string currentStreet: "Seseame street"
    signal navigationStarted()
    signal navigationStopped()
    signal navigationFinished()
    signal navigationManuver(string manuverDescription);

    function startNavigation() {
        navigation = true;
    }

    function stopNavigation() {
        navigation = false;
    }

    property Timer fakeNextManuver: Timer {
        property int count: 0
        property var manuvers: ['Turn left in 100 meters',
                                'Turn left now',
                                'Turn right in 200 meters',
                                'Turn right now',
                                'Follow the road for next 2.6 kilometers',
                                'Follow the road for next 2 kilometers',
                                'Follow the road for next 1.5 kilometers',
                                'Turn right soon',
                                'When possible, please turn around',
    ]
        running: navigation
        interval: 1000
        repeat: true
        onTriggered: {
            var str = manuvers[count++]
            navigationManuver(str)
        }
    }
}

