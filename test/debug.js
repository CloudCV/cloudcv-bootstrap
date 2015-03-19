var assert = require("assert")
var fs     = require('fs');
var inspect = require('util').inspect;

var cloudcv = require("../cloudcv.js");

var images = [
    'test/calibration_1/grid_01.jpg',
    'test/calibration_1/grid_02.jpg',
    'test/calibration_1/grid_03.jpg',
    'test/calibration_1/grid_04.jpg',
    'test/calibration_1/grid_05.jpg',
    'test/calibration_1/grid_06.jpg',
    'test/calibration_1/grid_07.jpg',
    'test/calibration_1/grid_08.jpg',
    'test/calibration_1/grid_09.jpg',
    'test/calibration_1/grid_10.jpg',
    'test/calibration_1/grid_11.jpg',
];
var patternSize = { width:9, height:6 };
cloudcv.calibrateCamera(images, patternSize, 'CHESSBOARD', function(error, result) {
    console.log(error, result);

    assert.equal(null, error);
    assert.notEqual(null, result);
    console.log(result.cameraMatrix.asObject());
    console.log(result.distCoeffs.asObject());
    done();
});