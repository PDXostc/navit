/**
 * Module responsible for handling communication with Navit.
 * Supported only on Tizen platform with special Crosswalk extension enabled.
 *
 * TODO:
 * - callbacks registering (and deregistering ?)
 *
 * @module navitGui.navit
 */
angular.module('navitGui.navit', [])
.factory('guinxe', ['$log', '$window', '$q', function ($log, $window, $q) {
    $log.log("Navit Communication Service::initialize");
    var isNxeSupported = false;

    if ($window.nxe) {
        isNxeSupported = true;
    }

    return {
        /**
         * Calls navit. Currently after a call which suppose to return image data,
         * there are always two responses.
         * First returns JSON, second returns image data encoded with base64.
         * So for such responses a json type response is treated as a deferred notify.
         *
         * @parama {object} data to be passed to nxe
         * @returns {deferred}
         */
        call: function (data) {
            $log.log("Navit Communication Service::Calling nxe", data);

            var deferred = $q.defer(),
                jsonData;

            if (!isNxeSupported) {
                deferred.reject("Navit Communication Service::Nxe is not supported!");
                return deferred.promise;
            }

            // convert data to json string
            jsonData = angular.toJson(data);

            // calling navit asynchronically
            nxe.render(jsonData, function (response) {
                var returnedData = {
                    image: false,
                    data: null
                };

                // handle JSON response
                if (response[0] === '{') {
                    response = angular.fromJson(response);
                    if (response.error) {
                        deferred.reject('Error '+ response.error);
                    } else {
                        returnedData.image = false;
                        returnedData.data = response.data;
                        deferred.notify(returnedData);
                    }
                } else {
                    //handling image data
                    returnedData.image = true;
                    returnedData.data = response;
                    deferred.resolve(returnedData);
                }
            });

            return deferred.promise;
        },
        /**
         * Similar to call, but resolves on json response instead of notify.
         * There is no image data for this type of request.
         *
         * @param {object} data
         * @returns {$q@call;defer.promise}
         */
        get: function (data) {
            $log.log("Navit Communication Service::Calling (get mode) nxe", data);

            var deferred = $q.defer(),
                jsonData;

            if (!isNxeSupported) {
                deferred.reject("Navit Communication Service::Nxe is not supported!");
                return deferred.promise;
            }

            // convert data to json string
            jsonData = angular.toJson(data);

            // calling navit asynchronically
            nxe.render(jsonData, function (response) {
                var returnedData = {
                    data: null
                };

                // handle JSON response
                if (response[0] === '{') {
                    response = angular.fromJson(response);
                    if (response.error) {
                        deferred.reject('Error '+ response.error);
                    } else {
                        returnedData.data = response.data;
                        deferred.resolve(returnedData);
                    }
                } else {
                    deferred.reject("Not a JSON response");
                }
            });

            return deferred.promise;
        },
        /**
         * For testing only purposes
         *
         * @param {type} data
         * @returns {$q@call;defer.promise}
         */
        _test: function (data) {
            var deferred = $q.defer();

            function hasProperCallName(d) {
                return d.call !== 'render' ? false : true;
            }

            setTimeout(function() {
                deferred.notify('About to send ', data);

                if (hasProperCallName(data)) {
                    deferred.resolve('Success render');
                } else {
                    deferred.reject('Failure for '+ data.call);
                }
            }, 2000);

          return deferred.promise;
        }
    };
}]);
