/**
 * Module responsible for resetting navit to factory defaults
 *
 * @module navitGui.reset
 */
angular.module( 'navitGui.reset', [
  'ui.router'
])
.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'reset', {
    url: '/reset',
    views: {
      "main": {
        controller: 'ResetCtrl',
        templateUrl: 'reset/reset.tpl.html'
      }
    },
    data:{
        pageTitle: 'Reset'
    }
  })
  .state( 'resetting', {
    url: '/resetting',
    views: {
      "main": {
        controller: 'ResetCtrl',
        templateUrl: 'reset/resetting.tpl.html'
      }
    },
    data:{
        pageTitle: 'Resetting'
    }
  });
})
.controller( 'ResetCtrl', function ResetController( $scope ) {
});