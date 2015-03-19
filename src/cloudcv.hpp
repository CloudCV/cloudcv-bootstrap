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

#pragma once

#include <nan.h>

namespace cloudcv
{
    NAN_METHOD(version);
    NAN_METHOD(buildInformation);
    NAN_METHOD(analyzeImage);
    NAN_METHOD(calibrationPatternDetect);
    NAN_METHOD(calibrateCamera);
}