/**
 * Module responsible for handling map display.
 *
 * @module navitGui.home
 */
angular.module( 'navitGui.home', [
  'ui.router',
  'navitGui.navit',
  'navitGui.map'
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

.run( function run ($log, $rootScope, guinxe) {
    $log.log("HomeCtrl::running...");

    //true=nort-up, false=head-up
    $rootScope.orientation = true;

    // setting proper icon for map orientation
    var promise = guinxe.get({
            id: 0,
            call: "orientation"
        });

    promise.then(
        function (response) {
            if (response.data) {
                $log.log("HomeCtrl::getOrientation::success");
                $log.log(response.data);
                $rootScope.orientation = response.data.orientation === -1 ? true : false;
            }
        },
        function (reason) {
            $log.log("HomeCtrl::Error while getOrientation::"+reason);
        }
    );

})

/**
 * And of course we define a controller for our route.
 */
.controller( 'HomeCtrl', function HomeController( $scope, $rootScope, $state, $window, $log, $timeout, guinxe, map) {

    var timeout = 500;

    /**
     * Loads a map for the firs time
     *
     * @returns {undefined}
     */
    function initializeMap () {
        $log.log("HomeCtrl::initializeMap::before nxe call");
        var promise = guinxe.call({
                id: 0,
                call: "render"
            });

        promise.then(
            function (response) {
                if (response.image) {
                    $log.log("HomeCtrl::initializeMap::success (image)");
                    // invoke map service
                    map.render(response.data);
                }
            },
            function (reason) {
                $log.log("HomeCtrl::Error when initializing a map::"+reason);
            },
            function (response) {
                // for now we skip json responses
                $log.log("HomeCtrl::initializeMap::notify (json) skipping"+response);
            }
        );

        $rootScope.mapInitialized = true;
    }

    /**
     * Text to Speech testing method
     *
     * @returns {undefined}
     */
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

    $scope.setOrientation = function () {
        $log.log("HomeCtrl::setOrientation::before nxe call");
        var orientation = $rootScope.orientation === true ? 0 : -1;
        var promise = guinxe.call({
                id: 0,
                call: "setOrientation",
                data: {
                    orientation: orientation
                }
            });

        promise.then(
            function (response) {
                if (response.image) {
                    $log.log("HomeCtrl::setOrientation::success (image)");
                    // invoke map service
                    map.render(response.data);
                }
            },
            function (reason) {
                $log.log("HomeCtrl::Error while setOrientation::"+reason);
            },
            function (response) {
                // here we can change orientation icon
                $log.log("HomeCtrl::setOrientation::notify (json) skipping", response);
                $rootScope.orientation = !$rootScope.orientation;

            }
        );
    };

    // hide or show location controls if in home.location state
    if($state.is('home.location')) {
        $scope.locationControls = {'visibility': 'visible'};
    } else {
        $scope.locationControls = {'visibility': 'hidden'};
    }

    // for now map is loaded after some time to properly calculate canvas
    // dimensions for tizen platform
    if (!$rootScope.mapInitialized) {
        $log.log('HomeCtrl::Trying to load a map after '+timeout+'ms.');
        $timeout(initializeMap, timeout);
    }


});
