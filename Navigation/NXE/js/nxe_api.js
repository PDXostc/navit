/*
 * NXE Javascript API
 */

var responseCallback = null;
var navitCanvas = null;

extension.setMessageListener(function(msg) {
    console.debug(msg.length);
    if (msg[0] === '{') {
        if (responseCallback instanceof Function) {
            console.debug('This is a normal callback');
            // this is a JSON
            var json = JSON.parse(msg);
            responseCallback(json)
        }
    } else {
        // this is a RAW byte from Render call
        var context = navitCanvas.getContext('2d');
        var img = new Image(1080,1900);
        console.debug('Try to display bmp');
        img.src="data:image/bmp;base64," + msg;
        img.onload = function() {
            context.drawImage(img,0,0);
        }
    }
});

exports.setRenderCanvas = function(canvas) {
    navitCanvas = canvas;
}

exports.render = function() {
    var message = {"call": "render", "id":0}
    return extension.postMessage(JSON.stringify(message));
}

exports.zoomBy = function( factor) {
    var zoomMessage = {"call":"zoomBy", "id":1, "data":{"factor":2}}
}
