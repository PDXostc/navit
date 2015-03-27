/**
 * Module responsible for applications settings.
 *
 * @module navitGui.settings
 */
angular.module( 'navitGui.settings', [
  'ui.router'
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

.run( function run ($window, $log) {

    var settings = JSON.parse($window.localStorage.getItem("settings"));
    $log.log("::Settings::getting settings from localStorage");
    $log.log("::Settings::value", settings);

    if (!settings) {
        $log.log("::Settings::creating settings in localStorage");
        $window.localStorage.setItem("settings", JSON.stringify({
            'poi': false
        }));
    }
})

.controller( 'SettingsCtrl', function SettingsCtrl( $scope, $window, $log) {

    //TODO:  this fragmet is called twice in settings.main state
    var settings = JSON.parse($window.localStorage.getItem("settings"));
    $log.log("::Settings::CONTROLLER::getting settings from localStorage");
    $log.log("::Settings::value", settings);

});