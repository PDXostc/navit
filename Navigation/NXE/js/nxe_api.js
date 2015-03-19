/*
 * NXE Javascript API
 */

var responseCallback = null;
var renderCallback = null;

extension.setMessageListener(function(msg) {
    if (echoListener instanceof Function) {
        if (msg[0] === '{') {
            // this is a JSON
            var json = JSON.parse(msg);
            responseCallback(json)
        } else {
            // this is a RAW byte from Render call
            renderCallback(msg);
        }
    };
});

exports.setRenderCallback = function(callback) {
    renderCallback = callback;
}

exports.render = function(callback) {
    responseCallback = callback;
    var message = {"call": "render", "id":0}
    return extension.postMessage(JSON.stringify(message));
}
