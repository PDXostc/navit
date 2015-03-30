/**
 * Module responsible displaying welcome screen which occurs when:
 * - no maps have been downloaded
 * - all maps have been deleted
 * 
 * @module navitGui.welcome
 */
angular.module( 'navitGui.welcome', [
  'ui.router'
])
.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'welcome', {
    url: '/welcome',
    views: {
      "main": {
        controller: 'WelcomeCtrl',
        templateUrl: 'welcome/welcome.tpl.html'
      }
    },
    data:{
        pageTitle: 'Welcome'
    }
  })
  .state( 'quit', {
    url: '/quit',
    views: {
      "main": {
        controller: 'WelcomeCtrl',
        templateUrl: 'welcome/quit.tpl.html'
      }
    },
    data:{
        pageTitle: 'Quit'
    }    
  });
})
.controller( 'WelcomeCtrl', function WelcomeController( $scope ) {
});