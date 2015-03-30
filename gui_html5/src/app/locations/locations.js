/**
 * Module responsible for managing locations, searching, faforites and history.
 * 
 * @module navitGui.locations
 */
angular.module( 'navitGui.locations', [
  'ui.router',
  'placeholders',
  'ui.bootstrap'
])

.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'locations', {
    url: '/locations',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/locations.tpl.html'
      }
    },
    data:{
        pageTitle: 'Locations'
    }
  })
  .state( 'locations.main', {
    url: '/main',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/main.tpl.html'
      }
    }
  })
  .state( 'locations.search', {
    url: '/search',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/search.tpl.html'
      }
    }
  })
  .state( 'locations.destination_history', {
    url: '/destination_history',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/destination_history.tpl.html'
      }
    }
  })
  .state( 'locations.favorites', {
    url: '/favorites',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/favorites.tpl.html'
      }
    }
  })
  .state( 'locations.results', {
    url: '/results',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/results.tpl.html'
      }
    }
  });
})

.controller( 'LocationsCtrl', function LocationsCtrl( $scope ) {
});