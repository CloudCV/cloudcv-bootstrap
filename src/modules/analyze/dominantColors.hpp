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
#include "framework/marshal/marshal.hpp"
#include <opencv2/opencv.hpp>
#include <set>

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
            ar & serialization::make_nvp("color", color);
            ar & serialization::make_nvp("totalPixels", totalPixels);
            ar & serialization::make_nvp("interclassVariance", interclassVariance);
            ar & serialization::make_nvp("error", error);
        }
    };

    std::ostream& operator<<(std::ostream& out, const Color& res);
    std::ostream& operator<<(std::ostream& out, const DominantColor& res);


    class DominantColorsExtractor
    {
    public:
        void process(const cv::Mat_<cv::Vec3b>& bgrImage);

        std::vector<DominantColor> mainColors;

        RGBDistribution getColorDeviation() const;

        int getUniqueColors() const;
        int getRedicedColors() const;

    protected:
        typedef std::set<int> ColorsSet;

        bool findLargestColorSet(int similarityTolerance, int minPixelsInSet, const std::set<Color>& input, std::set<Color>& colorsSet) const;
        DominantColor computeFinalColor(const std::set<Color>& colorsSet) const;

    private:
        std::vector<unsigned char> rVec, gVec, bVec;
        std::map<int, int> fullColorsTable;
        std::map<int, int> quantizedColorsTable;

    };

}
