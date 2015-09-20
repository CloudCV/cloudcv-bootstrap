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

    class HoughLinesAlgorithm : public Algorithm
    {
    public:

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
    };


    HoughLinesAlgorithmInfo::HoughLinesAlgorithmInfo()
        : AlgorithmInfo("houghLines",
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

        AlgorithmPtr HoughLinesAlgorithmInfo::create() const
        {
            return AlgorithmPtr(new HoughLinesAlgorithm());
        }

        NAN_METHOD(HoughLinesAlgorithmInfoProcess)
        {
        }

        Nan::FunctionCallback HoughLinesAlgorithmInfo::getFunction() const
        {
            return HoughLinesAlgorithmInfoProcess;
        }

}