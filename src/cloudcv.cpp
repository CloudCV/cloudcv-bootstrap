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
#include <nan-check.h>

using namespace cloudcv;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

#if TARGET_PLATFORM_UNIX || TARGET_PLATFORM_MAC
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

NAN_METHOD(getAlgorithms)
{
    std::vector<std::string> algorithms;

    for (auto alg : AlgorithmInfo::Get())
    {
        algorithms.push_back(alg.first);
    }
    
    info.GetReturnValue().Set(marshal(algorithms));
}

NAN_METHOD(getInfo)
{
    Nan::EscapableHandleScope scope;

    std::string algorithmName;
    std::string errorMessage;

    if (Nan::Check(info).ArgumentsCount(1)
        .Argument(0).IsString().Bind(algorithmName)
        .Error(&errorMessage))
    {
        auto algorithm = AlgorithmInfo::Get().find(algorithmName);
        if (algorithm == AlgorithmInfo::Get().end())
        {
            info.GetReturnValue().Set(Nan::Null());
            return;
        }

        info.GetReturnValue().Set(marshal(*algorithm->second.get()));
    }
    else
    {
        LOG_TRACE_MESSAGE(errorMessage);
        Nan::ThrowTypeError(errorMessage.c_str());
        return;
    }
}

NAN_METHOD(processFunction)
{
    Nan::HandleScope scope;

    std::string   algorithmName;
    std::string   errorMessage;
    v8::Local<v8::Object>   inputArguments;
    v8::Local<v8::Function> resultsCallback;

    if (Nan::Check(info).ArgumentsCount(3)
        .Argument(0).IsString().Bind(algorithmName)
        .Argument(1).IsObject().Bind(inputArguments)
        .Argument(2).IsFunction().Bind(resultsCallback)
        .Error(&errorMessage))
    {
        auto algorithm = AlgorithmInfo::Get().find(algorithmName);
        if (algorithm == AlgorithmInfo::Get().end())
        {
            v8::Local<v8::Value> argv[] = { Nan::Error("Algorithm not found"), Nan::Null() };
            Nan::Callback(resultsCallback).Call(2, argv);
            return;
        }

        ProcessAlgorithm(algorithm->second, inputArguments, resultsCallback);
    }
    else
    {
        LOG_TRACE_MESSAGE(errorMessage);
        Nan::ThrowTypeError(errorMessage.c_str());
        return;
    }
}


NAN_MODULE_INIT(RegisterModule)
{
#if TARGET_PLATFORM_UNIX || TARGET_PLATFORM_MAC
    signal(SIGSEGV, handler);   // install our handler
#endif

    AlgorithmInfo::Register(new HoughLinesAlgorithmInfo);

    Set(target,
        New<v8::String>("getAlgorithms").ToLocalChecked(),
        GetFunction(New<v8::FunctionTemplate>(getAlgorithms)).ToLocalChecked());

    Set(target,
        New<v8::String>("processFunction").ToLocalChecked(),
        GetFunction(New<v8::FunctionTemplate>(processFunction)).ToLocalChecked());

    Set(target,
        New<v8::String>("getInfo").ToLocalChecked(),
        GetFunction(New<v8::FunctionTemplate>(getInfo)).ToLocalChecked());
}

NODE_MODULE(cloudcv, RegisterModule);
