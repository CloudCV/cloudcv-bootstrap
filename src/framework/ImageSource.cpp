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
#include "Algorithm.hpp"
#include <nan.h>
#include "framework/marshal/marshal.hpp"

using namespace v8;
using namespace node;

namespace cloudcv
{
    class ImageSource::ImageSourceImpl
    {
    public:
        virtual ~ImageSourceImpl() = default;
        virtual cv::Mat getImage(int flags) = 0;
    };

    class FileImageSource : public ImageSource::ImageSourceImpl
    {
    public:
        FileImageSource(const std::string& imagePath)
            : mFilePath(imagePath)
        {            
        }

        cv::Mat getImage(int flags) override
        {
            return cv::imread(mFilePath, flags);
        }

    private:
        std::string mFilePath;
    };

    class BufferImageSource : public ImageSource::ImageSourceImpl
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

    ImageSource::ImageSource(std::shared_ptr<ImageSourceImpl> impl)
        : m_impl(impl)
    {
    }

    cv::Mat ImageSource::getImage(int flags /* = cv::IMREAD_COLOR */)
    {
        if (m_impl)
            m_impl->getImage(flags);

        throw std::runtime_error("Image is empty");
    }

    ImageSource ImageSource::CreateImageSource(const std::string& filepath)
    {
        return ImageSource(std::shared_ptr<ImageSourceImpl>(new FileImageSource(filepath)));
    }

    ImageSource ImageSource::CreateImageSource(v8::Local<v8::Value> bufferOrString)
    {
        if (Buffer::HasInstance(bufferOrString))
            return CreateImageSource(bufferOrString->ToObject());

        if (bufferOrString->IsString())
            return CreateImageSource(marshal<std::string>(bufferOrString->ToString()));

        throw serialization::MarshalTypeMismatchException("Invalid input argument type. Cannot create ImageSource");
    }

    ImageSource ImageSource::CreateImageSource(v8::Local<v8::Object> imageBuffer)
    {
        return ImageSource(std::shared_ptr<ImageSourceImpl>(new BufferImageSource(imageBuffer)));
    }

}