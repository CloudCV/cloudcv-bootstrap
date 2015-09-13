var DEBUG_ADDON = "./build/Debug/cloudcv";

var cloudcv = require(DEBUG_ADDON);


cloudcv.houghLines({ "image": "test/data/opencv-logo.jpg"}, function(error, result) {                    
    console.log((error));
    console.log((result));
    done();
});