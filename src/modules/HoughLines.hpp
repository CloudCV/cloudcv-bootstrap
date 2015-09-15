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

#pragma once

#include "framework/Algorithm.hpp"

namespace cloudcv
{
    class HoughLinesAlgorithm : public Algorithm
    {
    protected:
    public:
        static std::string name();
        AlgorithmInfoPtr info() override;

        void process(
            const std::map<std::string, ParameterBindingPtr>& inArgs,
            const std::map<std::string, ParameterBindingPtr>& outArgs
            ) override;

        static AlgorithmPtr create();
    };
}

NAN_METHOD(houghLines);
