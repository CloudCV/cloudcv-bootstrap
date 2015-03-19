#pragma once 

#include <nan.h>

namespace cloudcv {

    /**
     * @brief A base class for asynchronous task running in worker pool
     */
    class Job : public NanAsyncWorker
    {
    public:
        virtual ~Job();
		explicit Job(NanCallback *callback);

        void Execute();

        virtual void HandleOKCallback();

    protected:
        void SetErrorMessage(const std::string& errorMessage);
        
        virtual void ExecuteNativeCode() = 0;

		virtual v8::Local<v8::Value> CreateCallbackResult() = 0;

    private:
    };
}
