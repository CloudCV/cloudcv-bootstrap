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

var assert = require('assert');
var cloudcv = require("../cloudcv.js");

describe('cv', function() {
    it('buildInformation', function(done) {
        var info = cloudcv.buildInformation();
        assert.notEqual(null, info);
        done();
    });

    it('version check', function(done) {
        var ver = cloudcv.version();
        assert.notEqual(null, ver);
        done();
    });
});