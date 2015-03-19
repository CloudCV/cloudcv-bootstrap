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

 var express = require('express')
  , http = require('http')
  , async = require('async')
  , request = require('request')
  , validator = require('validator')
  , logger = require('./logger.js')
  , config = require('./config.js')
  , inspect = require('util').inspect
  ;


module.exports.plainDownloadViaHttpGet = function (url, callback) {
    
    logger.info('Downloading image from %s', url);
    
    var requestSettings = {
        method: 'GET',
        url: url,
        encoding: null
    };
    
    var payloadSize = 0;
    
    var req = request(requestSettings, function (error, response, body) {
      callback(error, body);
    }).on('data', function (chunk) {
      payloadSize += chunk.length;
      
      if (payloadSize > config.maxFileSize) {
        logger.warn('Maximum payload size exceed. Stopping download.');
        req.abort();
        callback(new Error('Maximum payload size exceed.'), null);
      }
    });
};