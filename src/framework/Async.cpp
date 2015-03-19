#include "framework/Async.hpp"
#include "framework/Job.hpp"
#include "framework/Logger.h"

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