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
#include "framework/NanCheck.hpp"
#include "framework/Job.hpp"
#include "framework/ImageSource.hpp"
#include "framework/Logger.hpp"
#include "framework/Algorithm.hpp"
#include "modules/HoughLines.hpp"
#include <vector>

namespace cloudcv
{
    static const char * InArgImage = "image";
    static const char * InArgRho = "rho";
    static const char * InArgTheta = "theta";
    static const char * InArgThreshold = "threshold";

    static const char * OutArgLines = "lines";

    class HoughLinesAlgorithmInfo : public AlgorithmInfo
    {
    public:

        static inline std::string name()
        {
            return "houghLines";
        }

        HoughLinesAlgorithmInfo()
            : AlgorithmInfo(name(),
            {
                { RequiredArgument<ImageSource>::Create(InArgImage) },
                { RangedArgument<float>::Create(InArgRho, 1, 2, 100) },
                { RangedArgument<float>::Create(InArgTheta, 1, 2, 100) },
                { RangedArgument<int>::Create(InArgThreshold, 1, 2, 255) }
        },
        {
            { TypedOutputArgument< std::vector<cv::Point2f> >::Create(OutArgLines) }
        }
            )
            {
        }
    };

    std::string HoughLinesAlgorithm::name()
    {
        return HoughLinesAlgorithmInfo::name();
    }

    AlgorithmInfoPtr HoughLinesAlgorithm::info()
    {
        static std::shared_ptr<AlgorithmInfo> _info;
        if (!_info)
            _info.reset(new HoughLinesAlgorithmInfo);

        return _info;
    }

    void HoughLinesAlgorithm::process(
        const std::map<std::string, ParameterBindingPtr>& inArgs,
        const std::map<std::string, ParameterBindingPtr>& outArgs
        )
    {
        TRACE_FUNCTION;
        ImageSource source = getInput<ImageSource>(inArgs, InArgImage);
        const float rho = getInput<float>(inArgs, InArgRho);
        const float theta = getInput<float>(inArgs, InArgTheta);
        const int threshold = getInput<int>(inArgs, InArgThreshold);
        cv::Mat inputImage = source.getImage(cv::IMREAD_GRAYSCALE);

        std::vector<cv::Point2f>&lines = getOutput<std::vector<cv::Point2f> >(outArgs, OutArgLines);

        cv::HoughLines(inputImage, lines, rho, theta, threshold);
        LOG_TRACE_MESSAGE("Detected " << lines.size() << " lines");
    }

    AlgorithmPtr HoughLinesAlgorithm::create()
    {
        return AlgorithmPtr(new HoughLinesAlgorithm());
    }
}

NAN_METHOD(houghLines)
{
    TRACE_FUNCTION;
    ProcessAlgorithm(cloudcv::HoughLinesAlgorithm::create(), info);
}