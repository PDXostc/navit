/**
 * Module responsible for discovering user actions and behavior like:
 * - quick double tap for zooming
 * - etc. to be continued
 *
 * @module behavior
 */
angular.module( 'behavior', [])
.factory('userActions', ['$timeout', '$log', '$q', function ($timeout, $log, $q) {

    /**
     * Possible actions, which name is returned to other handlers like nxeCall.
     * Object also stores some information which determines user actions performed.
     *
     * @type type
     */
    var actions = {
            zoom: {
                zin: {},
                zout: {},
                clicks: 0,
                lastClick: 0, // timestamp,
                interval: 300 //  beetwen clicks
            }
        },
        /**
         * Handles mousedown event and gathers data required for user action detection
         *
         * @returns {undefined}
         */
        zoomHandler = function () {
            actions.zoom.clicks += 1;
            actions.zoom.lastClick = Date.now();
            $log.log('Behavior::UserAction::click('+actions.zoom.clicks+')');

            $timeout(actionDetected, actions.zoom.interval);


        },
        /**
         * Checks if behavior detecting algorithm should be called
         *
         * @param {object} target DOM element
         * @returns {Boolean}
         */
        shouldReact = function (target) {
            var react = false;
            if (target.id === 'mapCanvas' || target.id === 'mainWindow') {
                react = true;
            }
            return react;
        },
        /**
         *  Checks if gathered data allows to fire user action
         *
         * @returns {undefined}
         */
        actionDetected = function () {

        };

    // service invocation
    return function ($event) {

        // TODO:
        //
        // jeśli chodzi o gesty click
        // to patrzymy priorytetowo
        // 1. najwyższy priorytet ma triple click

        if (shouldReact($event.target)) {
            $log.log('Behavior::UserAction::detected::analyzing...');

            // handle zoom in or zoom out
            if ($event.type === 'mousedown') {
                $log.log('Behavior::UserAction::gathering data... (not implemented)');
                //zoomHandler();
            }
        }
    };

}]);
