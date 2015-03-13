/**
 * Module responsible for applications settings.
 * 
 * @module navitGui.settings
 */
angular.module( 'navitGui.settings', [
  'ui.router',
  'placeholders',
  'ui.bootstrap'
])

.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'settings', {
    url: '/settings',
    views: {
      "main": {
        controller: 'SettingsCtrl',
        templateUrl: 'settings/settings.tpl.html'
      }
    },
    data:{
        pageTitle: 'Settings'
    }
  })
  .state( 'settings.main', {
    url: '/main',
    views: {
      "main": {
        controller: 'SettingsCtrl',
        templateUrl: 'settings/main.tpl.html'
      }
    }
  })
  .state( 'settings.countrylang', {
    url: '/country',
    views: {
      "main": {
        controller: 'SettingsCtrl',
        templateUrl: 'settings/country_lang.tpl.html'
      }
    }
  })
  .state( 'settings.managepoi', {
    url: '/manage_poi',
    views: {
      "main": {
        controller: 'SettingsCtrl',
        templateUrl: 'settings/manage_poi.tpl.html'
      }
    }
  });
})

.controller( 'SettingsCtrl', function SettingsCtrl( $scope ) {
});