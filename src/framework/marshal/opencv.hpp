#pragma once

#include <nan.h>
#include <nan-marshal.h>
#include <opencv2/opencv.hpp>

#include "framework/Logger.hpp"
#include "framework/ImageView.hpp"

namespace Nan
{
    namespace marshal
    {
        using namespace cloudcv;

        template <typename T>
        struct Serializer < cv::Size_<T> >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Size_<T>& val)
            {
                TRACE_FUNCTION;

                ar & make_nvp("width", val.width);
                ar & make_nvp("height", val.height);
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Size_<T>& val)
            {
                ar & make_nvp("width", val.width);
                ar & make_nvp("height", val.height);
            }
        };

        template <>
        struct Serializer < cv::String >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::String& val)
            {
                Nan::Utf8String cStr(ar);
                val = cv::String(*cStr, cStr.length());
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::String& val)
            {
                ar = Nan::New<v8::String>(val.c_str());
            }
        };

        template <typename T>
        struct Serializer < cv::Point_<T> >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Point_<T>& val)
            {
                ar & make_nvp("x", val.x);
                ar & make_nvp("y", val.y);
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Point_<T>& val)
            {
                ar & make_nvp("x", val.x);
                ar & make_nvp("y", val.y);
            }
        };

        template<typename T>
        struct Serializer < cv::Point3_<T> >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Point3_<T>& val)
            {
                ar & make_nvp("x", val.x);
                ar & make_nvp("y", val.y);
                ar & make_nvp("z", val.z);
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Point3_<T>& val)
            {
                ar & make_nvp("x", val.x);
                ar & make_nvp("y", val.y);
                ar & make_nvp("z", val.z);
            }
        };

        template<typename T>
        struct Serializer < cv::Rect_<T> >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Rect_<T>& val)
            {
                ar & make_nvp("x", val.x);
                ar & make_nvp("y", val.y);
                ar & make_nvp("width", val.width);
                ar & make_nvp("height", val.height);
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Rect_<T>& val)
            {
                ar & make_nvp("x", val.x);
                ar & make_nvp("y", val.y);
                ar & make_nvp("width", val.width);
                ar & make_nvp("height", val.height);
            }
        };

        template<>
        struct Serializer < cv::Scalar >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Scalar& val)
            {
                ar & val.val;
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Scalar& val)
            {
                ar & val.val;
            }
        };

        template<typename N, int cn>
        struct Serializer < cv::Vec<N, cn> >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Vec<N, cn>& val)
            {
                ar & val.val;
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Vec<N, cn>& val)
            {
                ar & val.val;
            }
        };

        template<>
        struct Serializer < cv::Mat >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, cv::Mat& val) = delete;

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const cv::Mat& val)
            {
                // TODO: Implement
                //ar = cloudcv::ImageView::ViewForImage(val);
            }
        };

        template<>
        struct Serializer < ImageView >
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, ImageView& val)
            {
                val = ImageView::CreateImageSource(ar.target());
            }

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const ImageView& val)
            {
                const cv::Mat& m = val.getImage();

                ar & make_nvp("rows", m.rows);
                ar & make_nvp("cols", m.cols);
                ar & make_nvp("channels", m.channels());
                ar & make_nvp("type", m.type());

                switch (m.type())
                {
                case CV_32F:
                {
                               auto start = m.ptr<float>(0);
                               auto end = start + m.total();
                               ar & make_nvp("data", std::vector<float>(start, end));
                               break;
                }
                case CV_8U:
                default:
                {
                           auto start = m.ptr<uint8_t>(0);
                           auto end = start + m.elemSize() * m.total();
                           ar & make_nvp("data", std::vector<uint8_t>(start, end));
                           break;
                }
                }
            }
        };
    }
}