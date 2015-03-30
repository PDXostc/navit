/**
 * Module responsible for applications settings.
 *
 * @module navitGui.settings
 */
angular.module( 'navitGui.settings', [
  'ui.router',
  'LocalStorageModule'
])

.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'settings', {
    url: '/settings',
    views: {
      "main": {
        // uncomment when breadcumbs introduced
        //controller: 'SettingsCtrl',
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

.run( function run ($rootScope, $log, localStorageService) {
    var settingsKeys = {
            'poi': 'off',
            'orientation': 'north-up',
            'view': '2D',
            'theme': 'basic',
            'routes': 'fast',
            'voice': 'on'
        },
        setKey;

    // initialize settings
    angular.forEach(settingsKeys, function(value, key){
        setKey = "settings." + key;
        if(!localStorageService.get(setKey)) {
            $log.log("::Settings::RUN::creating initial " + setKey);
            localStorageService.set(setKey, value);
        }
        // make settings be available everywhere
        localStorageService.bind($rootScope, setKey);
    });
})

.controller( 'SettingsCtrl', function SettingsCtrl($scope, $rootScope, $log) {

    // stores settings possible values
    $scope.states = {
        'poi': ['off', 'on'],
        'orientation': ['north-up'],
        'view': ['2D', '3D'],
        'theme': ['basic'],
        'routes': ['fast', 'short'],
        'voice': ['off', 'on']
    };

    /**
     * Controls toggling settings values
     * Param should be short e.g 'poi'
     *
     * @param {string} setting
     * @returns {undefined}
     */
    $scope.updateSettings = function(setting) {
        var value, index;
        // don't toggle single options
        if ($scope.states[setting].length === 1) {
            return;
        }
        // get current value
        value = $rootScope.settings[setting];
        // get index of current value
        index = $scope.states[setting].indexOf(value);
        // move to the next value or the begining
        if ($scope.states[setting].length > index + 1) {
            $rootScope.settings[setting] = $scope.states[setting][index +1];
        } else {
            $rootScope.settings[setting] = $scope.states[setting][0];
        }
    };

    $scope.settingOffColor = function (setting) {
        return $rootScope.settings[setting] === 'off';
    };


});