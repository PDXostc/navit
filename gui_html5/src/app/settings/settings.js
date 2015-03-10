angular.module( 'navitGui.settings', [
  'ui.router',
  'placeholders',
  'ui.bootstrap'
])

.config(function config( $stateProvider ) {
  $stateProvider.state( 'settings', {
    url: '/settings',
    views: {
      "main": {
        controller: 'SettingsCtrl',
        templateUrl: 'settings/settings.tpl.html'
      }
    },
    data:{ pageTitle: 'Settings' }
  });
})

.controller( 'SettingsCtrl', function AboutCtrl( $scope ) {
  // todo 
});