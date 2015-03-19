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

#include <cloudcv.hpp>
#include <modules/analyze/analyze.hpp>

#include <framework/marshal/marshal.hpp>
#include <framework/NanCheck.hpp>
#include <framework/Job.hpp>
#include <framework/ImageSource.hpp>
#include <framework/Logger.h>

#include <vector>

using namespace v8;
using namespace node;
using namespace cloudcv;



namespace cloudcv
{
    class ImageAnalyzeTask : public Job
    {
    public:

        ImageAnalyzeTask(ImageSourcePtr image, NanCallback * callback)
            : Job(callback)
            , m_imageSource(image)
        {
            TRACE_FUNCTION;
        }

        virtual ~ImageAnalyzeTask()
        {
            TRACE_FUNCTION;
        }

    protected:

        // This function is executed in another thread at some point after it has been
        // scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
        // will crash randomly and you'll have a lot of fun debugging.
        // If you want to use parameters passed into the original call, you have to
        // convert them to PODs or some other fancy method.
        virtual void ExecuteNativeCode()
        {
            TRACE_FUNCTION;
            cv::Mat image = m_imageSource->getImage();

            if (image.empty())
            {
                SetErrorMessage("Cannot decode image");
                return;
            }

            AnalyzeImage(image, m_analyzeResult);
        }

        // This function is executed in the main V8/JavaScript thread. That means it's
        // safe to use V8 functions again. Don't forget the HandleScope!
        virtual Local<Value> CreateCallbackResult()
        {
            TRACE_FUNCTION;
            NanEscapableScope();
            return NanEscapeScope(marshal(m_analyzeResult));
        }

    private:
        ImageSourcePtr m_imageSource;
        AnalyzeResult  m_analyzeResult;
    };

    NAN_METHOD(analyzeImage)
    {
        using namespace cloudcv;

        TRACE_FUNCTION;
        NanEscapableScope();
        
        Local<Object> imageBuffer;
        std::string   imageFile;
        Local<Function> imageCallback;
        std::string     error;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(2)
            .Argument(0).IsBuffer().Bind(imageBuffer)
            .Argument(1).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);
            NanAsyncQueueWorker(new ImageAnalyzeTask(CreateImageSource(imageBuffer), callback));
            NanReturnUndefined();
        }
        else if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(2)
            .Argument(0).IsString().Bind(imageFile)
            .Argument(1).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);
            NanAsyncQueueWorker(new ImageAnalyzeTask(CreateImageSource(imageFile), callback));
            NanReturnUndefined();
        }
        else if (!error.empty())
        {
            std::cout << "Cannot parse input arguments: " << error.c_str() << std::endl;
            NanThrowTypeError(error.c_str());
        }

        NanReturnUndefined();
    }


}