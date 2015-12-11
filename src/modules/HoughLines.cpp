/**********************************************************************************
 * CloudCV Bootstrap - A starter template for Node.js with OpenCV bindings.
 *                      This project lets you to quickly prototype a REST API
 *                    in a Node.js for a image processing service written in C++.
 *
 * Author: Eugene Khvedchenya <ekhvedchenya@gmail.com>
 *
 * More information:
 *  - https://cloudcv.io
 *  - http://computer-vision-talks.com
 *
 **********************************************************************************/

#include "framework/marshal/marshal.hpp"
#include "framework/Job.hpp"
#include "framework/ImageView.hpp"
#include "framework/Logger.hpp"
#include "framework/Algorithm.hpp"
#include "modules/HoughLines.hpp"
#include <vector>
#include <nan-check.h>

namespace cloudcv
{
    class HoughLinesAlgorithm : public Algorithm
    {
    public:
        struct image
        {
            static const char * name() { return "image"; };
            typedef ImageView type;
        };

        struct rho
        {
            static const char * name() { return "rho"; };
            typedef float type;
        };

        struct theta
        {
            static const char * name() { return "theta"; };
            typedef float type;
        };

        struct threshold
        {
            static const char * name() { return "threshold"; };
            typedef int type;
        };

        struct lines
        {
            static const char * name() { return "lines"; };
            typedef std::vector<cv::Point2f> type;
        };

        void process(
            const std::map<std::string, ParameterBindingPtr>& inArgs,
            const std::map<std::string, ParameterBindingPtr>& outArgs
            ) override
        {
            TRACE_FUNCTION;
            ImageView source = getInput<image>(inArgs);
            const float _rho = getInput<rho>(inArgs);
            const float _theta = getInput<theta>(inArgs);
            const int _threshold = getInput<threshold>(inArgs);
            cv::Mat inputImage = source.getImage(cv::IMREAD_GRAYSCALE);

            std::vector<cv::Point2f> &_lines = getOutput<lines>(outArgs);

            cv::HoughLines(inputImage, _lines, _rho, _theta, _threshold);
            LOG_TRACE_MESSAGE("Detected " << _lines.size() << " lines");
        }
    };

    

    HoughLinesAlgorithmInfo::HoughLinesAlgorithmInfo()
        : AlgorithmInfo("houghLines",
        {
            { inputArgument<HoughLinesAlgorithm::image>() },
            { inputArgument<HoughLinesAlgorithm::rho>(1, 2, 100) },
            { inputArgument<HoughLinesAlgorithm::theta>(1, 2, 100) },
            { inputArgument<HoughLinesAlgorithm::threshold>(1, 2, 255) }
        },
        {
            { outputArgument<HoughLinesAlgorithm::lines>() }
        }
        )
    {
    }

    AlgorithmPtr HoughLinesAlgorithmInfo::create() const
    {
        return AlgorithmPtr(new HoughLinesAlgorithm());
    }

}