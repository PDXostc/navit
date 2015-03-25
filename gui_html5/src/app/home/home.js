/**
 * Module responsible for handling map display.
 *
 * @module navitGui.home
 */
angular.module( 'navitGui.home', [
  'ui.router',
  'nxe'
])

/**
 * Each section or module of the site can also have its own routes. AngularJS
 * will handle ensuring they are all available at run-time, but splitting it
 * this way makes each module more "self-contained".
 */
.config(function config( $stateProvider ) {
  $stateProvider
  .state( 'home', {
    url: '/home',
    views: {
      "main": {
        controller: 'HomeCtrl',
        templateUrl: 'home/home.tpl.html'
      }
    },
    data:{
        pageTitle: 'Home',
        backgroundClass: 'backgroundMap'
    }
  })
  //state after clicking a location from search result
  .state( 'home.location', {
    url: '/location',
    views: {
      "main": {
        controller: 'HomeCtrl',
        templateUrl: 'home/home.tpl.html'
      }
    }
  });
})

/**
 * And of course we define a controller for our route.
 */
.controller( 'HomeCtrl', function HomeController( $scope, $state, $window, $log, nxeCall ) {

    // Text to speech testing
    $scope.say = function () {
        var text = 'After 50 meters turn left',
            msg;

        // I'm not sure if window.tizen is the proper way of checking API features
        if ($window.tizen && $window.tizen.speech) {
            msg = tizen.speech;
            msg.vocalizeString(text);
        } else {
            // this is only to work with other platforms like WebKit
            msg = new SpeechSynthesisUtterance(text);
            $window.speechSynthesis.speak(msg);
        }
    };

    $scope.sendNXE = function () {

        var canvas = document.getElementById('mapCanvas'),
            ctx;

        if (canvas && canvas.getContext){
            ctx = canvas.getContext('2d');

            nxeCall("render", function (data) {
                $log.log("Inside a callback... processing response");
                $log.log("Type of data", typeof data, "(length="+data.length+")");

                $log.log('Creating image data');
                var img = new Image(1080,1900);
                var dataUrl = "data:image/bmp;base64," + data;
                img.src = dataUrl;

                $log.log("Draw image in canvas");
                img.onload = function() {
                    ctx.drawImage(img, 0, 0);
                    $log.log('DONE');
                };
            });

        } else {
            $log.log("Canvas not created or not supported");
        }

    };


    // hide or show location controls if in home.location state
    if($state.is('home.location')) {
        $scope.locationControls = {'visibility': 'visible'};
    } else {
        $scope.locationControls = {'visibility': 'hidden'};
    }


});
