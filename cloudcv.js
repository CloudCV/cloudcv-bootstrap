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

var fs = require('fs');

var DEBUG_ADDON =  __dirname + "/build/Debug/cloudcv.node";
var RELEASE_ADDON = __dirname + "/build/Release/cloudcv.node";


if (fs.existsSync(RELEASE_ADDON))
    var nativeModule = require(RELEASE_ADDON);
else if (fs.existsSync(DEBUG_ADDON))
    var nativeModule = require(DEBUG_ADDON);
else
    throw new Error("Cannot find CloudCV addon");

var algorithms = nativeModule.getAlgorithms();

function registerAlgorithm(algName, index, array) {
  console.log('a[' + index + '] = ' + algName);

  module.exports[algName] = function(args, callback) { nativeModule.processFunction(algName, args, callback); };
} 

algorithms.forEach(registerAlgorithm);