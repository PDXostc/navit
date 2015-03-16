/**
 * Module responsible downloading maps
 * 
 * @module navitGui.download
 */
angular.module( 'navitGui.download', [
  'ui.router'
])

.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'download', {
    url: '/download',
    views: {
      "main": {
        controller: 'DownloadCtrl',
        templateUrl: 'download/download.tpl.html'
      }
    },
    data:{
        pageTitle: 'Download a map'
    }
  })
  .state( 'download.recommended', {
    url: '/recommended',
    views: {
      "main": {
        controller: 'DownloadCtrl',
        templateUrl: 'download/recommended.tpl.html'
      }
    }
  })
  .state( 'download.all', {
    url: '/all',
    views: {
      "main": {
        controller: 'DownloadCtrl',
        templateUrl: 'download/all.tpl.html'
      }
    }
  });
})
.controller( 'DownloadCtrl', function DownloadCtrl( $scope ) {
});
