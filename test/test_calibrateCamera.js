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

    describe('calibrateCamera', function() {
        
        it('detectChessboardGrid (Buffer)', function(done) {
            var imageData = fs.readFileSync('test/cameracalibration/chess_corners/chess1.png');
            assert.notEqual(null, imageData);

            var patternSize = { width:5, height:7 };

            cloudcv.calibrationPatternDetect(imageData, patternSize, 'CHESSBOARD', function(error, result) {

                assert.equal(null, error);
                assert.notEqual(null, result);
                assert.equal(true, result.patternFound);
                done();
            });
        });

        it('detectChessboardGrid (File)', function(done) {
            var patternSize = { width:5, height:7 };

            cloudcv.calibrationPatternDetect('test/cameracalibration/chess_corners/chess2.png', patternSize, 'CHESSBOARD', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                assert.equal(true, result.patternFound);
                done();
            });
        });

        it('detectCirclesGrid (Buffer)', function(done) {
            var imageData = fs.readFileSync('test/cameracalibration/circles/circles1.png');
            assert.notEqual(null, imageData);
            var patternSize = { width:7, height:7 };

            cloudcv.calibrationPatternDetect(imageData, patternSize, 'CIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                assert.equal(true, result.patternFound);
                done();
            }); 
        });

        it('detectCirclesGrid (File)', function(done) {
            var patternSize = { width:7, height:7 };

            cloudcv.calibrationPatternDetect('test/cameracalibration/circles/circles2.png', patternSize, 'CIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                assert.equal(true, result.patternFound);
                done();
            }); 
        });
        
        it('detectAsymmetricCirclesGrid (Buffer)', function(done) {
            var imageData = fs.readFileSync('test/cameracalibration/asymmetric_circles/acircles1.png');
            assert.notEqual(null, imageData);
            var patternSize = { width:7, height:13 };

            cloudcv.calibrationPatternDetect(imageData, patternSize, 'ACIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                assert.equal(true, result.patternFound);
                done();
            }); 
        });

        it('detectAsymmetricCirclesGrid (File)', function(done) {
            var patternSize = { width:7, height:13 };

            cloudcv.calibrationPatternDetect('test/cameracalibration/asymmetric_circles/acircles2.png', patternSize, 'ACIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                assert.equal(true, result.patternFound);
                done();
            }); 
        });

        it('calibrate_chessboard_grid', function(done) {

            this.timeout(10000);

            var images = [
                'test/cameracalibration/chess_corners/chess1.png',
                'test/cameracalibration/chess_corners/chess2.png',
                'test/cameracalibration/chess_corners/chess3.png',
                'test/cameracalibration/chess_corners/chess4.png',
                'test/cameracalibration/chess_corners/chess5.png',
                'test/cameracalibration/chess_corners/chess6.png',
                'test/cameracalibration/chess_corners/chess7.png',
            ];
            var patternSize = { width:5, height:7 };
            cloudcv.calibrateCamera(images, patternSize, 'CHESSBOARD', function(error, result) {

                assert.equal(null, error);
                assert.notEqual(null, result);

                var cam = result.cameraMatrix.asObject();
                var dis = result.distCoeffs.asObject();
                
                //console.log(cam);
                //console.log(dis);

                assert.notEqual(null, cam);
                assert.notEqual(null, dis);

                assert.equal(3, cam.size.width);
                assert.equal(3, cam.size.height);

                done();
            });
        });
        /**/
    });
});