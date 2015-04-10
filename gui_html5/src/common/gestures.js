/**
 * Module responsible for handling gestures for zooming.
 *
 * @module navitGui.gestures
 */
angular.module( 'navitGui.gestures', [
    'navitGui.navit',
    'navitGui.map'
])
.factory('gestures', ['$log', '$window', 'guinxe', 'map', function ($log, $window, guinxe, map) {
    $log.log("Gestures Service::initialize");

    $log.log('USE ONLY MOUSE ?', $window.useOnlyMouseInput);

    // default Hammer options
    var options = {
            inputClass: Hammer.MouseInput
        },
        // determine if running on tizen platform
        envTizen = $window.navigator.userAgent.match('Crosswalk') !== null ? true : false,
        initialized = false,
        canvas = null,
        mc = null,
        pinchIn = false,
        _handlers = {
            singletap: function (e) {
                $log.log('SINGLETAP SKIP THIS');
            },
            doubletap: function (e) {
                zoom(2);
            },
            tripletap: function (e) {
                zoom(-2);
            },
            pinchstart: function (e) {
                pinchIn = e.scale < 1 ? true : false;
            },
            pinchend: function (e) {
                if (pinchIn) {
                    $log.log('PinchIN - zoom out');
                    zoom(-2);
                } else {
                    $log.log('PinchOUT - zoom in');
                    zoom(2);
                }

            },
            panend: function (e) {
                var mapCenterX = Math.round(canvas.width / 2),
                    mapCenterY = Math.round(canvas.height / 2),
                    moveX = mapCenterX - e.deltaX,
                    moveY = mapCenterY - e.deltaY;

                    move(moveX, moveY);
            }
        };

    // change options based on environment
    if (envTizen && !$window.useOnlyMouseInput) {
        $log.log('Gestures Service::built with Touch only::switch input class to TouchInput');
        options.inputClass = Hammer.TouchInput;
    }


    /**
     * Zoom handler
     *
     * @param {number} factor if positive then zoom in otherwise zoom out
     * @returns {undefined}
     */
    function zoom (factor) {
        $log.log("Gestures Service::zoom::before nxe call");
        var promise = guinxe.call({
                id: 0,
                call: "zoomBy",
                data: {factor: factor}
            });

        promise.then(
            function (response) {
                if (response.image) {
                    $log.log("Gesture Service::zoom::success (image)");
                    // invoke map service
                    map.render(response.data);
                }
            },
            function (reason) {
                $log.log("Gesture Service::zoom::error::"+reason);
            },
            function (response) {
                // for now we skip json responses
                $log.log("Gesture Service::zoom::notify (json) skipping"+response);
            }
        );
    }

    /**
     * Move handler
     *
     * @param {number} x new map center X coordinate
     * @param {number} y new map center Y coordinate
     * @returns {undefined}
     */
    function move (x, y) {
        $log.log("Gestures Service::move::before nxe call");
        var promise = guinxe.call({
                id: 0,
                call: "moveBy",
                data: {
                    x: x,
                    y: y
                }
            });

        promise.then(
            function (response) {
                if (response.image) {
                    $log.log("Gesture Service::move::success (image)");
                    // invoke map service
                    map.render(response.data);
                }
            },
            function (reason) {
                $log.log("Gesture Service::move::error::"+reason);
            },
            function (response) {
                // for now we skip json responses
                $log.log("Gesture Service::move::notify (json) skipping"+response);
            }
        );
    }


    /**
     * Global handler which fires different handlers for different gesture events
     *
     * @param {object} e event object
     * @returns {undefined}
     */
    function _handleGesture (e) {
        if (e.target.id === 'mapCanvas' || e.target.id === 'mainWindow') {
            $log.log("Gesture Service::Detected gesture::" + e.type);
            $log.log(e);

            // try to invoke dedicated handler
            if (_handlers.hasOwnProperty(e.type)) {
                _handlers[e.type](e);
            } else {
                $log.log("Gesture Service::handler not found::for "+e.type);
            }
        }
    }

    return {
        setup: function (c, el) {
            var doubleTap, tripleTap, singleTap, pinch, pan;

            if (initialized) {
                $log.info("Gestures Service::already initialized");
                return;
            }
            canvas = c;
            initialized = true;
            mc = new Hammer.Manager(el, options);

            // TODO: try to experiment without this single tap gesture
            singleTap = new Hammer.Tap({event: 'singletap', taps: 1});
            doubleTap = new Hammer.Tap({event: 'doubletap', taps: 2});
            tripleTap = new Hammer.Tap({event: 'tripletap', taps: 3});
            pinch = new Hammer.Pinch({event: 'pinch'});
            pan = new Hammer.Pan({event: 'pan'});

            mc.add([tripleTap, doubleTap, singleTap, pinch, pan]);

            tripleTap.recognizeWith([doubleTap, singleTap]);
            doubleTap.recognizeWith(singleTap);

            doubleTap.requireFailure(tripleTap);
            singleTap.requireFailure([tripleTap, doubleTap]);

            // attach one handler for events
            // there is strange behavior of Hammer when multiple events attached
            // to one handle in single expression then multiple events reckognized
            // at the same time
            // below statement seems to work incorrectly
            //mc.on("doubletap tripletap pinchstart pinchend panend", _handleGesture);
            // but this works as expected

            mc.on("singletap", _handleGesture);
            mc.on("doubletap", _handleGesture);
            mc.on("tripletap", _handleGesture);
            mc.on("pinchstart", _handleGesture);
            mc.on("pinchend", _handleGesture);
            mc.on("panend", _handleGesture);
        }
    };
}]);
