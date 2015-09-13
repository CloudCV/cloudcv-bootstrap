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
#pragma once 

#include <node.h>
#include <v8.h>
#include <nan.h>

namespace cloudcv {

    /**
     * @brief A base class for asynchronous task running in worker pool
     */
    class Job : public Nan::AsyncWorker
    {
    public:
        virtual ~Job();
		explicit Job(Nan::Callback *callback);

        void Execute() override;

        virtual void HandleOKCallback() override;

    protected:
        void SetErrorMessage(const std::string& errorMessage);
        
        virtual void ExecuteNativeCode() = 0;

		virtual v8::Local<v8::Value> CreateCallbackResult() = 0;

    private:
    };
}
