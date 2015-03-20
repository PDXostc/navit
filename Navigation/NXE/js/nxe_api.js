/*
 * NXE Javascript API
 */

var renderListener = null;

extension.setMessageListener(function(msg) {
    console.log("Response length", msg.length);
    //console.log("Response", msg);

    // invoke a callback provided
    if (renderListener instanceof Function) {
        console.log('Invoking callback');
        renderListener(msg);
    }
});

exports.render = function(jsonMessage, callback) {
    renderListener = callback;
    return extension.postMessage(jsonMessage);
};
