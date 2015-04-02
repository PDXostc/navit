/**
 * Module responsible for handling communication with NXE.
 * Supported only on Tizen platform with special Crosswalk extension enabled.
 *
 * @module nxe
 */
angular.module( 'nxe', [])
.factory('nxeCall', ['$window', '$log', 'dateFilter', function ($window, $log, dateFilter) {
    $log.log("NXE Service::creating factory");

        /**
         *  Flag to determine that canvas has proper size
         *
         * @type Boolean|Boolean
         */
    var canvasResized = false,
        /**
         * Low level nxe call
         *
         * @param {JSON} data json string
         * @returns {undefined}
         */
        call = function (data) {
            $log.log("Calling nxe", data);
            try {
                if ($window.nxe) {
                    nxe.render(data, function (data) {
                        $log.log("Inside a callback... processing response");
                        $log.log("Type of data", typeof data, "(length="+data.length+")");
                        if (data[0] === '{') {
                           $log.log('Got JSON in respnse');
                        } else {
                            paintMap(data);
                        }
                    });
                } else {
                    $log.info('NXE is not supported');
                }
            } catch (err) {
                $log.error(err);
            }
        },
        /**
         * list of nxe handlers
         * render, zoomBy,...
         *
         */
        handlers = {
            render: function (params) {
                var jsonData = {};
                $log.log("Render handler invoked");

                resizeCanvas();

                jsonData.call = "render";
                jsonData.id = 0;
                call(JSON.stringify(jsonData));
            },
            zoomBy: function (params) {
                var jsonData = {};
                $log.log("zoomBy handler invoked");

                resizeCanvas();

                jsonData.call = "zoomBy";
                jsonData.id = 0;
                jsonData.data = params;
                call(JSON.stringify(jsonData));
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
        /**
         * Render image data as map in canvas
         *
         * @param {string} data base64 string
         * @returns {undefined}
         */
        paintMap = function (data) {
            var canvas = document.getElementById('mapCanvas'),
                ctx;

            if (canvas && canvas.getContext){
                $log.log('Creating image data');
                ctx = canvas.getContext('2d');
                printTime("TIME CALLBACK START=");
                var img = new Image(1080,1660);
                var dataUrl = "data:image/bmp;base64," + data;
                img.src = dataUrl;

                $log.log("Draw image in canvas");
                img.onload = function() {
                    ctx.drawImage(img, 0, 0);
                    printTime("TIME DRAW=");
                    $log.log('DONE');
                };
            } else {
                $log.log("Canvas not created or not supported");
            }

        },
        /**
         * Simpleutility function to display time with miliseconds
         *
         * @param {string} label
         * @returns {undefined}
         */
        printTime = function(label) {
            $log.log(label+dateFilter(new Date(), 'HH:mm:ss:sss'));
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
    return function (name, params) {
        $log.log("NXE service requested");
        try {
            handlers[name](params);
        } catch (err) {
            $log.error('Handler '+ name + ' is not supported!');
        }
    };

}]);
