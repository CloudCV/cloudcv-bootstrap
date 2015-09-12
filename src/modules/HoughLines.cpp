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

using namespace v8;
using namespace node;

namespace cloudcv
{
    class HoughLinesAlgorithmInfo : public AlgorithmInfo
    {
        AlgorithmParamPtr _input[4];
        AlgorithmParamPtr _output[1];

    public:
        HoughLinesAlgorithmInfo()
        {
            _input[0].reset(new TypedParameter<ImageSource>("image"));
            _input[1].reset(new TypedParameter<float>("rho", 1));
            _input[2].reset(new TypedParameter<float>("theta", 1));
            _input[3].reset(new TypedParameter<int>("threshold"));

            _output[0].reset(new TypedParameter< std::vector<cv::Point2f> >("lines"));
        }

        std::string name() const override
        {
            return "analyzeImage";
        }

        uint32_t inputArguments() const override
        {
            return 1;
        }

        uint32_t outputArguments() const override
        {
            return 1;
        }

        AlgorithmParamPtr getInputArgumentType(uint32_t argumentIndex) const override
        {
            switch (argumentIndex)
            {
            case 0:
                return _input[0];

            default:
                throw std::runtime_error("Invalid argument index");
            }
        }

        AlgorithmParamPtr getOutputArgumentType(uint32_t argumentIndex) const override
        {
            switch (argumentIndex)
            {
            case 0:
                return _output[0];

            default:
                throw std::runtime_error("Invalid argument index");
            }
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
            ImageSource source = getInput<ImageSource>(inArgs, "image");
            const float rho = getInput<float>(inArgs, "rho");
            const float theta = getInput<float>(inArgs, "theta");
            const int threshold = getInput<int>(inArgs, "threshold");

            std::vector<cv::Point2f>&lines = getOutput<std::vector<cv::Point2f> >(outArgs, "lines");

            cv::HoughLines(source.getImage(cv::IMREAD_GRAYSCALE), lines, rho, theta, threshold);
        }

        inline static AlgorithmPtr create()
        {
            return AlgorithmPtr(new HoughLinesAlgorithm());
        }
    };

    NAN_METHOD(houghLines)
    {
        ProcessAlgorithm(HoughLinesAlgorithm::create(), info);
    }
}