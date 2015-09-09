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

#include "modules/analyze/AnalyzeImageAlgorithm.hpp"
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
    class AnalyzeImageAlgorithmInfo : public AlgorithmInfo
    {
        AlgorithmParamPtr _input[1];
        AlgorithmParamPtr _output[1];

    public:
        AnalyzeImageAlgorithmInfo()
        {
            _input[1] = TypedParameter<cv::Mat>::create("input");
            _output[1] = TypedParameter<cv::Mat>::create("output");
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

    class AnalyzeImageAlgorithm : public Algorithm
    {
    public:
        AlgorithmInfoPtr info() override
        {
            static std::shared_ptr<AlgorithmInfo> _info;
            if (!_info)
                _info.reset(new AnalyzeImageAlgorithmInfo);

            return _info;
        }

        void process(const std::vector<ParameterBindingPtr>& inputArgs,
                     std::vector<ParameterBindingPtr>& outputArgs) override
        {
            cv::Mat source = dynamic_cast<TypedBinding<cv::Mat>*>(inputArgs[0].get())->get();
            cv::Mat& res = dynamic_cast<TypedBinding<cv::Mat>*>(outputArgs[1].get())->get();

            source.copyTo(res);
        }

        inline static AlgorithmPtr create()
        {
            return Algorithm::create("AnalyzeImageAlgorithm");
        }
    };

    NAN_METHOD(analyzeImage)
    {
        ProcessAlgorithm(AnalyzeImageAlgorithm::create(), args);
        NanReturnUndefined();
    }
}