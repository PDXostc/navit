/*
 * NXE Javascript API
 */

var responseCallback = null;
var navitCanvas = null;

extension.setMessageListener(function(msg) {
    console.debug(msg.length)
    if (msg[0] === '{') {
        if (responseCallback instanceof Function) {
            console.debug('This is a normal callback');
            // this is a JSON
            var json = JSON.parse(msg);
            responseCallback(json)
        }
    } else {
        console.debug('this is a response for render');
        // this is a RAW byte from Render call
        var context = navitCanvas.getContext('2d');
        var image = context.createImageData(1080, 1900);
        for (var i=0; i< 1080* 1900*4; i+=4) {
            image.data[i] = msg[i];
            image.data[i+1] = msg[i+1];
            image.data[i+2] = msg[i+2];
            image.data[i+3] = msg[i+3];
        }
        context.putImageData(image, 0,0);
    }
});

exports.setRenderCanvas = function(canvas) {
    navitCanvas = canvas;
}

exports.render = function() {
    var message = {"call": "render", "id":0}
    return extension.postMessage(JSON.stringify(message));
}
