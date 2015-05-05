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
#include "framework/Async.hpp"
#include "framework/Job.hpp"
#include "framework/Logger.hpp"

namespace cloudcv
{
    
    class AsyncTask : public Job
    {
    public:
        AsyncTask(AsyncLambdaFunction task, NanCallback * callback)
            : Job(callback)
            , mTask(task)
            , mReturnResult([](){ NanEscapableScope(); return NanEscapeScope(NanNull()); })
        {
        }

        virtual ~AsyncTask()
        {

        }

    protected:

        virtual v8::Local<v8::Value> CreateCallbackResult()
        {
            TRACE_FUNCTION;
            NanEscapableScope();
            return NanEscapeScope(mReturnResult());
        }

        virtual void ExecuteNativeCode()
        {
            TRACE_FUNCTION;
            AsyncErrorFunction errorResult = [this](const char * msg) {
                SetErrorMessage(msg);
            };

            AsyncReturnHelper returnResult(mReturnResult);
            mTask(returnResult, errorResult);
        }
    private:
        AsyncLambdaFunction                     mTask;
        std::function<v8::Local<v8::Value>()>   mReturnResult;
    };


    void Async(AsyncLambdaFunction task, NanCallback * callback)
    {
        TRACE_FUNCTION;
        NanAsyncQueueWorker(new AsyncTask(task, callback));
    }   
}