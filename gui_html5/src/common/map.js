/**
 * Module responsible for rendering a map.
 * Should only get data to render and nothing more.
 *
 * TODO:
 * - should canvas be resized from this module ?
 *
 * @module navitGui.map
 */
angular.module( 'navitGui.map', [])
.factory('map', ['$log', 'dateFilter', function ($log, dateFilter) {
    $log.log("Map Render Service::initialize");

    /**
     *  Flag to determine that canvas has proper size
     *
     * @type Boolean|Boolean
     */
    var canvasResized = false;

    /**
     * Simple utility function to display time with miliseconds
     *
     * @param {string} label
     * @returns {undefined}
     */
    function printTime (label) {
        $log.log(label+dateFilter(new Date(), 'HH:mm:ss:sss'));
    }

    /**
     * Debug document size for canvas resizing purposes
     *
     * @param {element} body
     * @param {element} html
     * @param {number} height
     * @returns {undefined}
     */
    function debugDocument (body, html, height) {
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
    }

    /**
     * Resize canvas just before rendering. This way canvas should always be
     * resized properly at the very last end.
     *
     * @returns {undefined}
     */
    function resizeCanvas (canvas) {
        var body, html, height;

        // don't resize again
        if (canvasResized) {
            return;
        }

        if (canvas) {
            $log.log('Map Render Service::resizeCanvas::to fit visible area.');

            //TODO: don't use native document variable, change to angular invocation
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
            $log.info('Map Render Service::resizeCanvas::Canvas is not created');
        }
    }

    return {
        /**
         * Render image data as map in canvas
         *
         * @param {string} data base64 string
         * @returns {undefined}
         */
        render: function (data) {
            var canvas = document.getElementById('mapCanvas'),
                ctx;

            // will be rsized only once
            if (!canvasResized) {
                resizeCanvas(canvas);
            }

            if (canvas && canvas.getContext){
                $log.log('Map Render Service::render::Creating image data');
                ctx = canvas.getContext('2d');
                printTime("Map Render Service::Draw Map START=");
                var img = new Image(1080,1660);
                var dataUrl = "data:image/bmp;base64," + data;
                img.src = dataUrl;

                $log.log("Map Render Service::render::Draw image in canvas");
                img.onload = function() {
                    ctx.drawImage(img, 0, 0);
                    printTime("Map Render Service::Draw Map FINISH=");
                };
            } else {
                $log.info('Map Render Service::render::Canvas is not created');
            }
        }
    };
}]);
