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
  'navitGui.gestures'
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

.run( function run ($rootScope, $log, $window, gestures) {
    $log.log("AppCtrl::running...");

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
        $log.log("AppCtrl::DOM loaded::creating gestures control");

        var canvas = document.getElementById('mapCanvas'),
            element = document.getElementById('applicationWrapper');

        gestures.setup(canvas, element);
    });

})

.controller( 'AppCtrl', function AppCtrl ( $scope, $location, $log) {
    $scope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
        if ( angular.isDefined( toState.data.pageTitle ) ) {
            $scope.pageTitle = toState.data.pageTitle + ' | navitGui' ;
        }
    });
});