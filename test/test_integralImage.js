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

    describe('integralImage', function() {
        
        it('getInfo', function(done) {
            console.log(JSON.stringify(cloudcv.getInfo('integralImage')));
            done();
        });

        it('process (File)', function(done) {
            cloudcv.integralImage({ "image": "test/data/opencv-logo.jpg"}, function(error, result) { 
                console.log(inspect(error));
                done();
            });
        });
        
        it('process (Buffer)', function(done) {
            var imageData = fs.readFileSync("test/data/opencv-logo.jpg");

            cloudcv.integralImage({ "image": imageData }, function(error, result) { 
                console.log(inspect(error));
                done();
            });
        });       


    });
});
