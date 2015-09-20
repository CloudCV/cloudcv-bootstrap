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

#include "framework/marshal/marshal.hpp"
#include "modules/HoughLines.hpp"

using namespace cloudcv;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_METHOD(getAlgorithms)
{
    std::vector<std::string> algorithms;

    for (auto alg : AlgorithmInfo::Get())
    {
        algorithms.push_back(alg.first);
    }
    
    info.GetReturnValue().Set(marshal(algorithms));
}

NAN_MODULE_INIT(RegisterModule)
{
    Set(target,
        New<v8::String>("getAlgorithms").ToLocalChecked(),
        GetFunction(New<v8::FunctionTemplate>(getAlgorithms)).ToLocalChecked());

    for (auto alg : AlgorithmInfo::Get())
    {
        Set(target,
            New<v8::String>(alg.first).ToLocalChecked(),
            GetFunction(New<v8::FunctionTemplate>(alg.second->getFunction())).ToLocalChecked());
    }
}

NODE_MODULE(cloudcv, RegisterModule);
