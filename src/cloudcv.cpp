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

#include "framework/ImageView.hpp"
#include "cloudcv.hpp"

using namespace v8;
using namespace node;
using namespace cloudcv;

void RegisterModule(Handle<Object> target)
{
    NODE_SET_METHOD(target, "houghLines", houghLines);

    ImageView::Init(target);
}

NODE_MODULE(cloudcv, RegisterModule);
