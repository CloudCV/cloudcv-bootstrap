#pragma once

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>
#include <map>
#include <node.h>
#include <v8.h>
#include <nan.h>

#include "framework/Argument.hpp"

namespace cloudcv
{
    class AlgorithmInfo
    {
    public:
        inline std::string name() const
        {
            return m_name;
        }

        inline const std::map<std::string, InputArgumentPtr>& inputArguments() const
        {
            return m_inputParams;
        }

        inline const std::map<std::string, OutputArgumentPtr>& outputArguments() const
        {
            return m_outputParams;
        }

    protected:
        AlgorithmInfo(
            const std::string& name,
            std::initializer_list<std::pair<std::string, InputArgumentPtr>> in,
            std::initializer_list<std::pair<std::string, OutputArgumentPtr>> out);

    private:
        std::string                              m_name;
        std::map<std::string, InputArgumentPtr>  m_inputParams;
        std::map<std::string, OutputArgumentPtr> m_outputParams;

    };
}