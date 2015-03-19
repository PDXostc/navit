/*
 * NXE Javascript API
 */

var responseCallback = null;
var navitCanvas = null;

extension.setMessageListener(function(msg) {
    if (echoListener instanceof Function) {
        if (msg[0] === '{') {
            // this is a JSON
            var json = JSON.parse(msg);
            responseCallback(json)
        } else {
            // this is a RAW byte from Render call
            var context = navitCanvas.getContext('2d');
            var image = context.createImageData(1080, 1900);
            image.data = msg;
            context.putImageData(image, 0,0);
        }
    };
});

exports.setRenderCanvas = function(canvas) {
    navitCanvas = canvas;
}

exports.render = function() {
    var message = {"call": "render", "id":0}
    return extension.postMessage(JSON.stringify(message));
}
