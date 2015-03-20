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
  'ui.router'
])

.config( function myAppConfig ( $stateProvider, $urlRouterProvider ) {
  $urlRouterProvider.otherwise( '/home' );
})

.run( function run ($rootScope, $log) {
    $log.log("navitGui starting...");
    
    // change background if map shouldn't be displayed
    $rootScope.$on('$stateChangeSuccess',function(event, toState){
        $rootScope.backgroundClass = toState.data.backgroundClass;
    });

})

.controller( 'AppCtrl', function AppCtrl ( $scope, $location ) {
  $scope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
    if ( angular.isDefined( toState.data.pageTitle ) ) {
      $scope.pageTitle = toState.data.pageTitle + ' | navitGui' ;
    }
  });
});