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

#include "modules/IntegralImage.hpp"
#include "framework/Algorithm.hpp"
#include "framework/ImageView.hpp"
#include "framework/Logger.hpp"

namespace cloudcv
{
    class IntegralImageAlgorithm : public Algorithm
    {
    public:
        struct image
        {
            static const char * name() { return "image"; };
            typedef ImageView type;
        };

        struct integralImage
        {
            static const char * name() { return "integralImage"; };
            typedef ImageView type;
        };
        

        void process(
            const std::map<std::string, ParameterBindingPtr>& inArgs,
            const std::map<std::string, ParameterBindingPtr>& outArgs
            ) override
        {
            TRACE_FUNCTION;
            ImageView _image = getInput<image>(inArgs);
            ImageView &_integralImage = getOutput<integralImage>(outArgs);

            cv::integral(_image.getImage(cv::IMREAD_GRAYSCALE), _integralImage.getImage());
        }
    };



    IntegralImageAlgorithmInfo::IntegralImageAlgorithmInfo()
        : AlgorithmInfo("integralImage",
        {
            { inputArgument<IntegralImageAlgorithm::image>() }
    },
    {
        { outputArgument<IntegralImageAlgorithm::integralImage>() }
    }
        )
        {
    }

        AlgorithmPtr IntegralImageAlgorithmInfo::create() const
        {
            return AlgorithmPtr(new IntegralImageAlgorithm());
        }



}