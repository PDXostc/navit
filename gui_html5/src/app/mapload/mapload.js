/**
 * Module responsible for map loading
 * 
 * @module navitGui.mapload
 */
angular.module( 'navitGui.mapload', [
  'ui.router'
])
.config(function config( $stateProvider ) {
  $stateProvider.state( 'mapload', {
    url: '/mapload',
    views: {
      "main": {
        controller: 'MapLoadCtrl',
        templateUrl: 'mapload/mapload.tpl.html'
      }
    },
    data:{
        pageTitle: 'Map loading'
    }
  });
})
.controller( 'MapLoadCtrl', function MaploadController( $scope ) {
});