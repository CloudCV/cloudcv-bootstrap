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
  , swagger = require("./lib/swaggerSpec.js");
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
app.set('views', __dirname + '/views');
app.set('view engine', 'jade');
app.set('view options', { pretty: true, layout: false });

app.use(methodOverride());
app.use(multer(multerOptions));
app.use(cookieParser('optional secret string'));
app.use(pmx.expressErrorHandler());
app.use(express.static(path.join(__dirname, 'public')));

// Static pages

var api_docs = [];
var algs = cv.getAlgorithms();
for (var i = algs.length - 1; i >= 0; i--)
{
    var info = cv.getInfo(algs[i]);
    api_docs.push(info);
    console.log(info.name)
};





app.get('/', function (req, res) { res.render('index'); });
app.get('/docs', function (req, res) { res.render('docs', { api: api_docs}); });
app.get('/docs/:algorithmName', function (req, res) { res.render('docs', { api: api_docs, currentAlgorithm: req.params.algorithmName}); });

// Specifications:
app.get('/swagger.json',  function (req, res) { res.json(swagger.getSpec(algs)); });

// Peace to everyone!
app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    next();
});

logger.info("cloudcv-bootstrap server starting on port " + app.get('port'));

http.createServer(app).listen(app.get('port'), function(){
  logger.info("cloudcv-bootstrap server listening on port " + app.get('port'));
});