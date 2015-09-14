var DEBUG_ADDON = __dirname + "/build/Debug/cloudcv.node";

var cloudcv = require(DEBUG_ADDON);


cloudcv.houghLines({ "image": "test/data/opencv-logo.jpg"}, function(error, result) {                    
    console.log((error));
    console.log((result));
    done();
});