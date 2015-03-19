/**
 * Module responsible for handling communication with NXE.
 * Supported only on Tizen platform with special Crosswalk extension.
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
                nxe.echoAsync(data, callback);
            } catch (err) {
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
    };

    // service constructor function
    return function (name, callback) {
        $log.log("NXE service requested");
        try {
            handlers[name](callback);
        } catch (err) {
            $log.error('Handler '+ name + ' is not supported!');
        }
    };

}]);
