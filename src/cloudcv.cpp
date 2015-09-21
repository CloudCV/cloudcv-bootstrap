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

NAN_METHOD(processFunction)
{
    Nan::HandleScope scope;

    
    if (info.Length() != 3)
    {
        LOG_TRACE_MESSAGE("Got " + std::to_string(info.Length()) + " arguments instead of 2");
        Nan::ThrowTypeError("This function should be called with 3 arguments: algorithm name, input and callback");
        return;
    }

    if (!info[0]->IsString())
    {
        LOG_TRACE_MESSAGE("First argument should be a string with algorithm name");
        Nan::ThrowTypeError("First argument should be a string with algorithm name");
        return;
    }

    if (!info[1]->IsObject())
    {
        LOG_TRACE_MESSAGE("Second argument should be an object");
        Nan::ThrowTypeError("Second argument should be an object");
        return;
    }

    if (!info[2]->IsFunction())
    {
        LOG_TRACE_MESSAGE("Incorrect type of second argument");
        Nan::ThrowTypeError("Thirds argument should be a function callback");
        return;
    }

    std::string   algorithmName = marshal<std::string>(info[0].As<v8::String>());
    v8::Local<v8::Object>   inputArguments = info[1].As<v8::Object>();
    v8::Local<v8::Function> resultsCallback = info[2].As<v8::Function>();

    auto algorithm = AlgorithmInfo::Get().find(algorithmName);
    if (algorithm == AlgorithmInfo::Get().end())
    {
        v8::Local<v8::Value> argv[] = { Nan::Error("Algorithm not found"), Nan::Null() };
        Nan::Callback(resultsCallback).Call(2, argv);
        return;
    }

    ProcessAlgorithm(algorithm->second, inputArguments, resultsCallback);
}


NAN_MODULE_INIT(RegisterModule)
{
    AlgorithmInfo::Register(new HoughLinesAlgorithmInfo);

    Set(target,
        New<v8::String>("getAlgorithms").ToLocalChecked(),
        GetFunction(New<v8::FunctionTemplate>(getAlgorithms)).ToLocalChecked());

    Set(target,
        New<v8::String>("processFunction").ToLocalChecked(),
        GetFunction(New<v8::FunctionTemplate>(processFunction)).ToLocalChecked());
}

NODE_MODULE(cloudcv, RegisterModule);
