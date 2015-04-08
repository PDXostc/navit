/**
 * Main application module, parent for all modules
 *
 * @module navitGui
 */
angular.module( 'navitGui', [
  'templates-app',
  'templates-common',
  'navitGui.home',
  'navitGui.settings',
  'navitGui.locations',
  'navitGui.welcome',
  'navitGui.download',
  'navitGui.mapload',
  'navitGui.reset',
  'ui.router',
  'LocalStorageModule',
  'nxe'
])

.config( function myAppConfig ( $stateProvider, $urlRouterProvider, $compileProvider, localStorageServiceProvider) {
    // let's rewrite valid href pattern to include 'app' on crosswalk
    $compileProvider.aHrefSanitizationWhitelist(/^\s*(https?|file|ftp|mailto|app):/);

    // go to home state when url not reckognized
    $urlRouterProvider.otherwise( '/home' );

    // set global prefix for localStorage keys and storage type
    localStorageServiceProvider
        .setPrefix('navit')
        .setStorageType('localStorage');

})

.run( function run ($rootScope, $log, $window, nxeCall) {
    $log.log("navitGui starting...");

    $rootScope.mapInitialized = false;

    // change background if map shouldn't be displayed
    $rootScope.$on('$stateChangeSuccess',function(event, toState){
        $rootScope.backgroundClass = toState.data.backgroundClass;

        // show canvas only for home state
        if(toState.name === 'home' || toState.name === 'home.location') {
            $rootScope.mapVisible = {'display': 'inline-block'};
        } else {
            $rootScope.mapVisible = {'display': 'none'};
        }
    });

    // setting up user gestures, Hammer needs to be used after DOM ready
    $window.addEventListener('load', function() {
        var canvas = document.getElementById('mapCanvas');
        $log.log("AppCtrl::DOM loaded::creating Hammer gestures control");

        var myElement = document.getElementById('applicationWrapper'),
            mc = new Hammer.Manager(myElement),
            doubleTap = new Hammer.Tap({event: 'doubletap', taps: 2 }),
            tripleTap = new Hammer.Tap({event: 'tripletap', taps: 3 }),
            pinch = new Hammer.Pinch({event: 'pinch'}),
            pan = new Hammer.Pan({event: 'pan'}),
            pinchIn = true;

        mc.add([tripleTap, doubleTap, pinch, pan]);
        tripleTap.requireFailure(doubleTap);

        //TODO: maybe its better to pass only event name
        //and nxe will handle the rest ?
        mc.on("doubletap", function(ev) {
            if (ev.target.id === 'mapCanvas' || ev.target.id === 'mainWindow') {
                $log.log("Double tap detected");
                nxeCall("zoomBy", {factor: 2});
            }
        });

        mc.on("tripletap", function(ev) {
            if (ev.target.id === 'mapCanvas' || ev.target.id === 'mainWindow') {
                $log.log("Triple tap detected");
                nxeCall("zoomBy", {factor: -2});
            }
        });

        mc.on("pinchstart", function(ev) {
            if (ev.target.id === 'mapCanvas' || ev.target.id === 'mainWindow') {
                //discover if it is pinch out or pinch in
                pinchIn = ev.scale < 1 ? true : false;
            }
        });

        mc.on("pinchend", function(ev) {
            if (ev.target.id === 'mapCanvas' || ev.target.id === 'mainWindow') {
                //discover if it is pinch out or pinch in
                if (pinchIn) {
                    $log.log('PinchIN - zoom out');
                    nxeCall("zoomBy", {factor: -2});

                } else {
                    $log.log('PinchOUT - zoom in');
                    nxeCall("zoomBy", {factor: 2});
                }
            }
        });

        mc.on("panend", function(ev) {
            if (ev.target.id === 'mapCanvas' || ev.target.id === 'mainWindow') {
                var mapCenterX = Math.round(canvas.width / 2),
                    mapCenterY = Math.round(canvas.height / 2),
                    moveX = mapCenterX - ev.deltaX,
                    moveY = mapCenterY - ev.deltaY;

                nxeCall("moveBy", {
                    x: moveX,
                    y: moveY
                });
            }
        });

    });

})

.controller( 'AppCtrl', function AppCtrl ( $scope, $location, $log) {
    $scope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
        if ( angular.isDefined( toState.data.pageTitle ) ) {
            $scope.pageTitle = toState.data.pageTitle + ' | navitGui' ;
        }
    });
});