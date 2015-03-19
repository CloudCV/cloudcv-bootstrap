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

#include "modules/common/Numeric.hpp"
#include "modules/analyze/dominantColors.hpp"
#include "framework/marshal/marshal.hpp"
#include <opencv2/opencv.hpp>
#include <array>

namespace cloudcv
{

    /**
    * Result of image analyze
    */
    struct AnalyzeResult
    {
        cv::Size                    frameSize;		// Size of input image       
        cv::Size                    aspectRatio;	// Aspect ratio of the input image

        Distribution                intensity;		// Distribution of the gray scale image intensity
        float                       rmsContrast;    // RMS contrast measure
        std::array<cv::Scalar, 255> histogram;
        int                         uniqieColors;
        int                         reducedColors;
        std::vector<DominantColor> dominantColors;
        RGBDistribution            colorDeviation;

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

    void AnalyzeImage(cv::Mat input, AnalyzeResult& value);

}