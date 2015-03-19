#include "Job.hpp"
#include <stdexcept>
#include <iostream>
#include "framework/Logger.h"

namespace cloudcv {

    Job::Job(NanCallback *callback)
        : NanAsyncWorker(callback)
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
        NanEscapableScope();

        v8::Local<v8::Value> argv[] = {
            NanNull(),
            CreateCallbackResult()
        };

        callback->Call(2, argv);
    }

    void Job::SetErrorMessage(const std::string& errorMessage)
    {
        LOG_TRACE_MESSAGE("Error message:" << errorMessage);

        NanAsyncWorker::SetErrorMessage(errorMessage.c_str());
    }
}
