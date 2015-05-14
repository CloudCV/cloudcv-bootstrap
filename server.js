/**********************************************************************************
 * CloudCV Boostrap - A starter template for Node.js with OpenCV bindings.
 *                    This project lets you to quickly prototype a REST API
 *                    in a Node.js for a image processing service written in C++. 
 * 
 * Author: Eugene Khvedchenya <ekhvedchenya@gmail.com>
 * 
 * More information:
 *  - https://cloudcv.io
 *  - http://computer-vision-talks.com
 * 
 **********************************************************************************/
 
var pmx = require('pmx'); pmx.init();

var express = require('express')
  , http = require('http')
  , path = require('path')
  , fs = require('fs')
  , cookieParser = require('cookie-parser')
  , multer     = require('multer')
  , methodOverride = require('method-override')
  , async = require('async')
  , request = require('request')
  , cv = require('./cloudcv.js')
  , validator = require('validator')
  , logger = require('./lib/logger.js')
  , download = require('./lib/download.js')
  , config = require('./lib/config.js')
  , inspect = require('util').inspect
  ;

var app = express();

var multerOptions = {
    inMemory:true, 
    limits: { 
        fileSize: config.maxFileSize, 
        files: 1
    }
}

// Configuration
app.set('port', process.env.PORT || 3000);
app.set('title', 'cloudcv-bootstrap');
app.use(methodOverride());
app.use(multer(multerOptions));
app.use(cookieParser('optional secret string'));
app.use(pmx.expressErrorHandler());  

function MapAnalyzeResult(analyzeResult) {
    var returnRes = {
        aspect: analyzeResult.aspectRatio,
        size: { width:0, height:0},
        dominantColors: []
    };

    for (var i = 0; i < analyzeResult.dominantColors.length; i++)
    {
        var c = analyzeResult.dominantColors[i];

        returnRes.dominantColors.push(c);
    }

    return returnRes;
}

// Peace to everyone!

app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    
    next();
});

app.get('/api/v1/buildInformation', function (req, res) {
    res.write(cv.buildInformation());
    res.end();
});

app.get('/api/v1/image/analyze/dominantColors', function (req, res) {

    var sendErrorResponse = function (code, message) {
        res.statusCode = code;
        res.setHeader("Content-Type", "application/json");
        res.write(JSON.stringify({message:message}));
        res.end();
    };

    if (!req.query.image) {
        return sendErrorResponse(404, 'Missing required parameter');
    }

    var externalImageURL = decodeURIComponent(req.query.image);

    if (!validator.isURL(externalImageURL)) {
        logger.warn('Given request is not an URL %s', externalImageURL);
        return sendErrorResponse(404, 'Missing required parameter');
    }

    async.waterfall([
        function (callback) {
            download.plainDownloadViaHttpGet(externalImageURL, callback);
        },
        function (image, callback) {
            logger.info('Loaded file from %s of %d bytes', externalImageURL, image.length);
            cv.analyzeImage(image, callback);
        }
        ], 
        function (failureOrNull, result) {
            
            console.log(failureOrNull, result);

            if (failureOrNull) {
                logger.error('Problem with a file %s: %s', externalImageURL, failureOrNull.message);
                sendErrorResponse(500, failureOrNull.message);
            }
            else if (result) {
                res.setHeader("Content-Type", "application/json");
                res.write(JSON.stringify(MapAnalyzeResult(result)));
                res.end();
            } 
            else {
                console.warn("Should not get here");
                res.end();
            }
        }
    );
});

app.post('/api/v1/image/analyze/dominantColors/', function (req, res) {

    var sendErrorResponse = function (code, message) {
        res.statusCode = code;
        res.setHeader("Content-Type", "application/json");
        res.write(JSON.stringify({message:message}));
        res.end();
    };

    if (req.files && req.files.image) {
        var uploadedImage = req.files.image.buffer;
    }
    else {
        return sendErrorResponse(400, "Missing image data");
    }
    
    async.waterfall([
        function (callback) {
            cv.analyzeImage(uploadedImage, callback); 
        }
        ], 
        function (failureOrNull, result) {

            if (failureOrNull) {
                logger.error('Problem with a file %s: %s', req.files.image.originalname, failureOrNull.message);
                sendErrorResponse(500, failureOrNull.message);
            }
            else if (result) {
                res.setHeader("Content-Type", "application/json");
                res.write(JSON.stringify(MapAnalyzeResult(result)));
                res.end();
            }
        }
    );

});

http.createServer(app).listen(app.get('port'), function(){
  logger.log("api.cloudcv.io server listening on port " + app.get('port'));
});