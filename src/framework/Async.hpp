#pragma once

#include <nan.h>
#include <functional>

namespace cloudcv 
{   
    class AsyncReturnHelper
    {
    public:
        AsyncReturnHelper(std::function<v8::Local<v8::Value>()>& target)
            : mTarget(target)
        {

        }

        template <typename T>
        void operator()(const T& value)
        {
            mTarget = [this, value]() {
                NanEscapableScope();
                return NanEscapeScope(marshal(value));                            
            };
        }

        template <typename T>
        void operator()(std::function<v8::Local<v8::Value>()> x)
        {
            mTarget = x;
        }

    private:
        AsyncReturnHelper(const AsyncReturnHelper& other);
        AsyncReturnHelper& operator=(const AsyncReturnHelper& other);

    private:
        std::function<v8::Local<v8::Value>()>& mTarget;
    };

    typedef std::function<void(const char*)> AsyncErrorFunction;

    /**
     * @brief Define the signature of the asynchronous task.
     * 
     * @param result  This argument can be used to 'return' result of processing into callback.
     *                Callback arguments can be passed as single objects or as a lambda function.
     *                See AsyncReturnHelper for more details.
     *                
     * @param error - If you encounter error inside task routine, you can use this argument to
     *                indicate error description and mark error  state. 
     *                Error state will be passed as a first argument into callback.
     *                See AsyncErrorFunction for more details.
     */
    typedef std::function<void (AsyncReturnHelper& result, AsyncErrorFunction error)> AsyncLambdaFunction;


    /**
     * @brief   Execute the lambda function in a parallel thread in a non-blocking 
     *          fashion and calls given callback when it's done.
     * 
     * @param   task     Asynchronous task.
     * @param   callback Callback function that will be called when task completed.
     */
    void Async(AsyncLambdaFunction task, NanCallback * callback);
}
