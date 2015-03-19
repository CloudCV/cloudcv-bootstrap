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

#include <opencv2/opencv.hpp>
#include <stdint.h>

namespace cloudcv
{
    size_t gcd(size_t u, size_t v);

    struct Distribution
    {
        float min;
        float max;
        float average;
        float standardDeviation;
        float entropy;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar & serialization::make_nvp("min", min);
            ar & serialization::make_nvp("max", max);
            ar & serialization::make_nvp("average", average);
            ar & serialization::make_nvp("standardDeviation", standardDeviation);
            ar & serialization::make_nvp("entropy", entropy);
        }
    };

    struct RGBDistribution
    {
        Distribution r;
        Distribution g;
        Distribution b;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar & serialization::make_nvp("r", r);
            ar & serialization::make_nvp("g", g);
            ar & serialization::make_nvp("b", b);
        }
    };

    Distribution distribution(cv::InputArray data);

    std::ostream& operator<<(std::ostream& out, const Distribution& res);
    std::ostream& operator<<(std::ostream& out, const RGBDistribution& res);

}
