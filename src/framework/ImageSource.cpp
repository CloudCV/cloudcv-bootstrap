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
#include "ImageSource.hpp"
#include <nan.h>

using namespace v8;
using namespace node;

namespace cloudcv
{
    class FileImageSource : public ImageSource
    {
    public:
        FileImageSource(const std::string& imagePath)
            : mFilePath(imagePath)
        {            
        }

        virtual ~FileImageSource()
        {
        }

        cv::Mat getImage(int flags)
        {
            return cv::imread(mFilePath, flags);
        }

    private:
        std::string mFilePath;
    };

    class BufferImageSource : public ImageSource
    {
    public:
        BufferImageSource(Local<Object> imageBuffer)
        {
            NanAssignPersistent(mImageBuffer, imageBuffer);

            mImageData    = Buffer::Data(imageBuffer);
            mImageDataLen = Buffer::Length(imageBuffer);
        }

        virtual ~BufferImageSource()
        {
            NanDisposePersistent(mImageBuffer);
        }

        cv::Mat getImage(int flags)
        {
            return cv::imdecode(cv::_InputArray(mImageData, mImageDataLen), flags);
        }

    private:
        Persistent<Object>       mImageBuffer;
        char                   * mImageData;
        size_t                   mImageDataLen;
    };

    ImageSourcePtr CreateImageSource(const std::string& filepath)
    {
        return ImageSourcePtr(new FileImageSource(filepath));
    }

    ImageSourcePtr CreateImageSource(v8::Local<v8::Object> imageBuffer)
    {
        return ImageSourcePtr(new BufferImageSource(imageBuffer));
    }

}