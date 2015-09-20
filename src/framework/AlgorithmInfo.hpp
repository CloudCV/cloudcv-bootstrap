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
    class Algorithm;
    class AlgorithmInfo;
    typedef std::shared_ptr<AlgorithmInfo> AlgorithmInfoPtr;

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
        
        virtual std::shared_ptr<Algorithm> create() const = 0;

        static void Register(AlgorithmInfoPtr info);
        static const std::map<std::string, AlgorithmInfoPtr>& Get();

        virtual Nan::FunctionCallback getFunction() const = 0;


    protected:
        AlgorithmInfo(
            const std::string& name,
            std::initializer_list<std::pair<std::string, InputArgumentPtr>> in,
            std::initializer_list<std::pair<std::string, OutputArgumentPtr>> out);

    private:
        std::string                              m_name;
        std::map<std::string, InputArgumentPtr>  m_inputParams;
        std::map<std::string, OutputArgumentPtr> m_outputParams;

        static std::map<std::string, AlgorithmInfoPtr> m_algorithms;
    };
}