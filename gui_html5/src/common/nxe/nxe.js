/**
 * Module responsible for handling communication with NXE.
 * Supported only on Tizen platform with special Crosswalk extension enabled.
 *
 * @module nxe
 */
angular.module( 'nxe', [])
.factory('nxeCall', ['$window', '$log', function ($window, $log) {


    /**
     * Low level nxe call
     *
     * @param {JSON} data json string
     * @param {function} callback
     * @returns {undefined}
     */
    var canvasResized = false,
        call = function (data, callback) {
            $log.log("Calling nxe", data);
            try {
                if ($window.nxe) {
                    nxe.render(data, callback);
                } else {
                    $log.info('NXE is not supported');
                }
            } catch (err) {
                $log.error(err);
            }
        },
        // template data to be send using nxe call
        jsonData = {
            id: null,
            call: ""
        },
        /**
         * list of nxe handlers
         * render, zoomBy,...
         *
         */
        handlers = {
            render: function (callback) {
                $log.log("Render handler invoked");

                resizeCanvas();

                jsonData.call = "render";
                jsonData.id = 0;
                call(JSON.stringify(jsonData), callback);
            }
        },
        /**
         * Resize canvas just before rendering. This way canvas should always be
         * resized properly at the very last end.
         *
         * @returns {undefined}
         */
        resizeCanvas = function () {
            if (canvasResized) {
                return;
            }
            //TODO: don't use native document variable, change to angular invocation
            var canvas = document.getElementById("mapCanvas"),
                body, html, height;

            if (canvas) {
                $log.log('Resizing canvas to fit visible area.');

                body = document.body;
                html = document.documentElement;

                // calculate canvas height and width
                height = Math.max( body.scrollHeight, body.offsetHeight,
                         html.clientHeight, html.scrollHeight, html.offsetHeight);
                //debugDocument(body, html, height);

                canvas.height = height - 260; // without simulated dna bars
                canvas.width = body.clientWidth;

                // set flag to prevent resizing again, because height or width
                // when touched will remove canvas content (at least on tizen)
                canvasResized = !canvasResized;
            } else {
                $log.log('Canvas is not created');
            }
        },
        debugDocument = function (body, html, height) {
            $log.log("BODY H (client)="+body.clientHeight);
            $log.log("BODY H (scroll)="+body.scrollHeight);
            $log.log("BODY H (offset)="+body.offsetHeight);
            $log.log("HTML H (client)="+html.clientHeight);
            $log.log("HTML H (scroll)="+html.scrollHeight);
            $log.log("HTML H (offset)="+html.offsetHeight);

            $log.log("BODY W (client)="+body.clientWidth);
            $log.log("BODY W (scroll)="+body.scrollWidth);
            $log.log("BODY W (offset)="+body.offsetWidth);
            $log.log("HTML W (client)="+html.clientWidth);
            $log.log("HTML W (scroll)="+html.scrollWidth);
            $log.log("HTML W (offset)="+html.offsetWidth);
            $log.log("BODY H (calculated)="+height);
        };


    // service invocation
    return function (name, callback) {
        $log.log("NXE service requested");
        try {
            handlers[name](callback);
        } catch (err) {
            $log.error('Handler '+ name + ' is not supported!');
        }
    };

}]);
