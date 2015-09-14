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

#include "cloudcv.hpp"

using namespace cloudcv;
using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_METHOD(getAlgorithms)
{
    std::vector<std::string> algorithms = {
        HoughLinesAlgorithm::name()
    };

    return marshal(algorithms);
}

NAN_MODULE_INIT(RegisterModule)
{
    Set(target,
        New<String>("getAlgorithms").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(getAlgorithms)).ToLocalChecked());

    Set(target,
        New<String>("houghLines").ToLocalChecked(), 
        GetFunction(New<FunctionTemplate>(houghLines)).ToLocalChecked());
}

NODE_MODULE(cloudcv, RegisterModule);
