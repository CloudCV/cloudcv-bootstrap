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

#include <nan.h>

#include "cloudcv.hpp"

using namespace v8;
using namespace node;
using namespace cloudcv;
using namespace Nan;

NAN_MODULE_INIT(RegisterModule)
{
    Set(target, New<String>("houghLines").ToLocalChecked(), GetFunction(New<FunctionTemplate>(houghLines)).ToLocalChecked());
}

NODE_MODULE(cloudcv, RegisterModule);
