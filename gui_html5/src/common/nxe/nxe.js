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
    var call = function (data, callback) {
            $log.log("Calling nxe", data);
            try {
                nxe.render(data, callback);
            } catch (err) {
                $log.log(err);
                $log.error('NXE is not supported');
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
                jsonData.call = "render";
                jsonData.id = 0;
                call(JSON.stringify(jsonData), callback);
            }
        },
        createCanvas = function () {
            var canvas = document.getElementById("mapCanvas"),
                idAttr = null;

            if (!canvas) {
                $log.log('Canvas is not created... creating.');
                canvas = document.createElement('canvas');
                idAttr = document.createAttribute('id');
                idAttr.value = 'mapCanvas';
                canvas.setAttributeNode(idAttr);
                // insert canvas as a first child of body element
                document.body.insertBefore(canvas, document.body.childNodes[0]);
                $log.log('Canvas has been created with id=mapCanvas');
            } else {
                $log.log('Canvas is already created');
            }
        };


    // initialize service internals, canvas, etc
    createCanvas();

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
