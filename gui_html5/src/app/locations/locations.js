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
    },
    data: {
        breadcrumb: 'Search',
        backState: 'locations.main'
    }
  })
  .state( 'locations.destination_history', {
    url: '/destination_history',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/destination_history.tpl.html'
      }
    },
    data: {
        breadcrumb: 'Destination History',
        backState: 'locations.main'
    }
  })
  .state( 'locations.favorites', {
    url: '/favorites',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/favorites.tpl.html'
      }
    },
    data: {
        breadcrumb: 'Favorites',
        backState: 'locations.main'
    }
  })
  .state( 'locations.results', {
    url: '/results',
    views: {
      "main": {
        controller: 'LocationsCtrl',
        templateUrl: 'locations/results.tpl.html'
      }
    },
    data: {
        breadcrumb: 'Search',
        backState: 'locations.main'
    }
  });
})

.controller( 'LocationsCtrl', function LocationsCtrl( $scope, $state, $rootScope ) {
    // set back state
    $rootScope.backUrl = '#/home';
    if ($state.current.data && $state.current.data.backState) {
        $rootScope.backUrl  = $state.href($state.current.data.backState);
    }

    // set breadcrumbs label
    $rootScope.breadcrumb = null;
    if ($state.current.data && $state.current.data.breadcrumb) {
        $rootScope.breadcrumb = $state.current.data.breadcrumb;
    }

});