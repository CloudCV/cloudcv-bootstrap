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

#include "framework/marshal/marshal.hpp"

#include <opencv2/opencv.hpp>
#include <array>

namespace cloudcv
{

    struct Color
    {
        int hash;
        int count;

        Color() : hash(0), count(0) {}
        Color(int h, int c) : hash(h), count(c) {}

        int distanceTo(const Color& other) const;
        int distanceTo(const cv::Vec3b& other) const;

        bool operator< (const Color& other) const
        {
            return hash < other.hash;
        }

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar & serialization::make_nvp("hash", hash);
            ar & serialization::make_nvp("count", count);
        }

    };

    struct DominantColor
    {
        cv::Vec3b color;
        int       totalPixels;

        float     interclassVariance;
        float     error;

        // Html string representation like #FFFFFF
        std::string html() const;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            using namespace cloudcv::serialization;
            ar & make_nvp("color", color);
            ar & make_nvp("totalPixels", totalPixels);
            ar & make_nvp("interclassVariance", interclassVariance);
            ar & make_nvp("error", error);
        }
    };

    /**
    * Result of image analyze
    */
    struct AnalyzeResult
    {
        cv::Size                    frameSize;		// Size of input image       
        cv::Size                    aspectRatio;	// Aspect ratio of the input image

        float                       rmsContrast;    // RMS contrast measure
        std::array<cv::Scalar, 255> histogram;
        int                         uniqieColors;
        int                         reducedColors;
        std::vector<DominantColor> dominantColors;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar & serialization::make_nvp("frameSize",      frameSize);
            ar & serialization::make_nvp("aspectRatio",    aspectRatio);
            ar & serialization::make_nvp("intensity",      intensity);
            ar & serialization::make_nvp("rmsContrast",    rmsContrast);
            ar & serialization::make_nvp("histogram",      histogram);
            ar & serialization::make_nvp("uniqieColors",   uniqieColors);
            ar & serialization::make_nvp("reducedColors",  reducedColors);
            ar & serialization::make_nvp("dominantColors", dominantColors);
            ar & serialization::make_nvp("colorDeviation", colorDeviation);
        }
    };

    std::ostream& operator<<(std::ostream& out, const AnalyzeResult& res);
    std::ostream& operator<<(std::ostream& out, const Color& res);
    std::ostream& operator<<(std::ostream& out, const DominantColor& res);

    void AnalyzeImage(cv::Mat input, AnalyzeResult& value);

}