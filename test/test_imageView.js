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

    describe('ImageView', function() {
        it('Create from image sync', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-logo.jpg");
            assert.equal(599, imview.width());
            assert.equal(555, imview.height());
            done();
        });

        it('Create from image async', function(done) {
            cloudcv.loadImage("test/data/opencv-logo.jpg", function(err, imview) {
                assert.equal(599, imview.width());
                assert.equal(555, imview.height());
                done();                
            });
        });

        it('asJpegStream', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-logo.jpg");
            imview.asJpegStream(function(err, data) {
                assert.equal(null, err);
                assert.notEqual(null, data);
                done();
            });
        });

        it('asJpegDataUri', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-logo.jpg");
            imview.asJpegDataUri(function(err, data) {
                assert.equal(null, err);
                assert.notEqual(null, data);
                done();
            });
        });

        it('asPngStream', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-logo.jpg");
            imview.asPngStream(function(err, data) {
                assert.equal(null, err);
                assert.notEqual(null, data);
                done();
            });
        });

        it('asPngDataUri', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-logo.jpg");
            imview.asPngDataUri(function(err, data) {
                assert.equal(null, err);
                assert.notEqual(null, data);
                done();
            });
        });

        it('asObject', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-small.png");
            var object = imview.asObject();
            assert.notEqual(null, object);
            done();
        });

        it('thumbnail(128,128)', function(done) {
            var imview = new cloudcv.ImageView("test/data/opencv-logo.jpg");
            imview.thumbnail(128, 128, function(error, thumbnail) {
                assert.equal(error, null, error);
                assert.equal(128, thumbnail.width());
                assert.equal(128, thumbnail.height());
                done();
            });
        });

    });
});