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
#include <node.h>
#include <v8.h>
#include <nan.h>

#include "framework/Logger.hpp"
#include "ImageView.hpp"
#include "Algorithm.hpp"
#include "framework/marshal/marshal.hpp"




namespace cloudcv
{
    class ImageView::ImageSourceImpl
    {
    public:
        ImageSourceImpl()
        {
        }

        ImageSourceImpl(cv::Mat image)
        {
            image.copyTo(m_holder);
        }

        virtual ~ImageSourceImpl() = default;

        inline const cv::Mat& getImage() const
        {
            return m_holder;
        }
        
        inline cv::Mat& getImage()
        {
            return m_holder;
        }
    private:
        cv::Mat m_holder;
    };
    
    ImageView::ImageView()
        : m_impl(std::shared_ptr<ImageSourceImpl>(new ImageSourceImpl()))
    {

    }

    ImageView::ImageView(std::shared_ptr<ImageSourceImpl> impl)
        : m_impl(impl)
    {
    }

    const cv::Mat& ImageView::getImage() const
    {
        if (m_impl.get() != nullptr)
        {
            return m_impl->getImage();
        }

        throw std::runtime_error("Image is empty");
    }

    cv::Mat& ImageView::getImage()
    {
        if (m_impl.get() != nullptr)
        {
            return m_impl->getImage();
        }

        throw std::runtime_error("Image is empty");
    }

    cv::Mat ImageView::getImage(int flags /* = cv::IMREAD_COLOR */) const
    {
        const cv::Mat& src = getImage();

        cv::Mat result;

        switch (flags)
        {
        case cv::IMREAD_GRAYSCALE:
            if (src.channels() == 3 || src.channels() == 4)
                cv::cvtColor(src, result, cv::COLOR_RGB2GRAY);
            else if (src.channels() == 1)
                result = src;
            else
                throw std::runtime_error("Cannot convert image to grayscale");
            break;

        case cv::IMREAD_COLOR:
            if (src.channels() == 3 || src.channels() == 4)
                result = src;
            else if (src.channels() == 1)
                cv::cvtColor(src, result, cv::COLOR_GRAY2RGB);
            else
                throw std::runtime_error("Cannot convert image to RGB");
            break;

        case cv::IMREAD_UNCHANGED:
        default:
            result = src;
            break;
        }

        return result;
    }


    ImageView ImageView::CreateImageSource(v8::Local<v8::Value> bufferOrString)
    {
        if (node::Buffer::HasInstance(bufferOrString))
            return CreateImageSource(bufferOrString->ToObject());

        if (bufferOrString->IsString())
            return CreateImageSource(marshal<std::string>(bufferOrString->ToString()));

        throw serialization::MarshalTypeMismatchException("Invalid input argument type. Cannot create ImageSource");
    }

    ImageView ImageView::CreateImageSource(v8::Local<v8::Object> imageBuffer)
    {
        LOG_TRACE_MESSAGE("ImageSource [Buffer]");        
        auto mImageData = node::Buffer::Data(imageBuffer);
        auto mImageDataLen = node::Buffer::Length(imageBuffer);

        cv::Mat m = cv::imdecode(cv::_InputArray(mImageData, (int)mImageDataLen), cv::IMREAD_UNCHANGED);
        return ImageView(std::shared_ptr<ImageSourceImpl>(new ImageSourceImpl(m)));
    }

    ImageView ImageView::CreateImageSource(const std::string& filepath)
    {
        LOG_TRACE_MESSAGE("ImageSource [File]:" << filepath);
        cv::Mat m = cv::imread(filepath, cv::IMREAD_UNCHANGED);
        return ImageView(std::shared_ptr<ImageSourceImpl>(new ImageSourceImpl(m)));
    }

}