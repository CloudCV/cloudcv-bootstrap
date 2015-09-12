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
#include "framework/Job.hpp"
#include "framework/Logger.hpp"

#include <stdexcept>
#include <iostream>

namespace cloudcv {

    Job::Job(Nan::Callback *callback)
        : Nan::AsyncWorker(callback)
    {
    }

    Job::~Job()
    {
    }

    void Job::Execute()
    {
        try
        {
            ExecuteNativeCode();
        }
        catch (cv::Exception& exc)
        {
            SetErrorMessage(exc.what());
        }
        catch (std::runtime_error& e)
        {
            SetErrorMessage(e.what());
        }
    }

    void Job::HandleOKCallback()
    {
        Nan::HandleScope scope;

        v8::Local<v8::Value> argv[] = {
            Nan::Null(),
            CreateCallbackResult()
        };

        callback->Call(2, argv);
    }

    void Job::SetErrorMessage(const std::string& errorMessage)
    {
        LOG_TRACE_MESSAGE("Error message:" << errorMessage);

        Nan::AsyncWorker::SetErrorMessage(errorMessage.c_str());
    }
}
