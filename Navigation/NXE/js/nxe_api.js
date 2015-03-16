/*
 * NXE Javascript API
 */

var echoListener = null;

extension.setMessageListener(function(msg) {
      if (echoListener instanceof Function) {
              echoListener(msg);
                };
});

exports.echoAsync = function (msg, callback) {
      echoListener = callback;
        extension.postMessage(msg);
};

exports.echoSync = function (msg) {
      return extension.internal.sendSyncMessage(msg);
};
