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

#include <vector>

namespace cloudcv
{
    static const char * InArgImage = "image";
    static const char * InArgRho = "rho";
    static const char * InArgTheta = "theta";
    static const char * InArgThreshold = "theta";
    static const char * OutArgLines = "lines";

    class HoughLinesAlgorithmInfo : public AlgorithmInfo
    {
    public:

        HoughLinesAlgorithmInfo()
            : AlgorithmInfo("houghLines",
            {
                { TypedParameter<ImageSource>::Create(InArgImage) },
                { TypedParameter<float>::Create(InArgRho, 2) },
                { TypedParameter<float>::Create(InArgTheta, 2) },
                { TypedParameter<int>::Create(InArgThreshold, 5) }
        },
        {
            { OutputParameter< std::vector<cv::Point2f> >::Create(OutArgLines) }
        }
            )
            {

        }
    };

    class HoughLinesAlgorithm : public Algorithm
    {
    protected:
    public:
        AlgorithmInfoPtr info() override
        {
            static std::shared_ptr<AlgorithmInfo> _info;
            if (!_info)
                _info.reset(new HoughLinesAlgorithmInfo);

            return _info;
        }

        void process(
            const std::map<std::string, ParameterBindingPtr>& inArgs,
            const std::map<std::string, ParameterBindingPtr>& outArgs
            ) override
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

        inline static AlgorithmPtr create()
        {
            return AlgorithmPtr(new HoughLinesAlgorithm());
        }
    };

    NAN_METHOD(houghLines)
    {
        TRACE_FUNCTION;
        ProcessAlgorithm(HoughLinesAlgorithm::create(), info);
    }
}