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

var assert = require("assert")
var fs     = require('fs');
var inspect = require('util').inspect;

var cloudcv = require("../cloudcv.js");

describe('cv', function() {

    describe('houghLines', function() {
        
        //console.log(cloudcv.houghLines);
        it('process (File)', function(done) {
            cloudcv.houghLines({ "image": "test/data/opencv-logo.jpg"}, function(error, result) { 
                   
                console.log(inspect(error));
                console.log(inspect(result));
                done();
            });
        });
        
        it('process (Buffer)', function(done) {
            var imageData = fs.readFileSync("test/data/opencv-logo.jpg");

            cloudcv.houghLines({ "image": imageData }, function(error, result) { 
                console.log(inspect(error));
                console.log(inspect(result));
                done();
            });
        });       

        it('shouldReturnError (Missing argument)', function(done) {

            cloudcv.houghLines({}, function(error, result) { 
                console.log(inspect(error));
                console.log(inspect(result));
                done();
            });
        });  

        it('shouldReturnError (Incorrect type)', function(done) {

            cloudcv.houghLines({ "image": 2}, function(error, result) { 
                console.log(inspect(error));
                console.log(inspect(result));
                done();
            });
        });  

    });
});
