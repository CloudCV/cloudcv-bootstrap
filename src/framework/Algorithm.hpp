#pragma once

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>
#include <map>
#include <node.h>
#include <v8.h>
#include <nan.h>

#include "framework/AlgorithmExceptions.hpp"
#include "framework/AlgorithmInfo.hpp"
#include "framework/Argument.hpp"

#include "framework/ImageSource.hpp"
#include "framework/marshal/marshal.hpp"
#include "framework/marshal/opencv.hpp"


namespace cloudcv
{
    class ParameterBinding;
    class AlgorithmInfo;

    typedef std::shared_ptr<AlgorithmInfo> AlgorithmInfoPtr;
    typedef std::shared_ptr<InputArgument> InputArgumentPtr;
    typedef std::shared_ptr<OutputArgument> OutputArgumentPtr;
    typedef std::shared_ptr<ParameterBinding> ParameterBindingPtr;

    class Algorithm
    {
    public:
        virtual ~Algorithm() = default;
        virtual void process(
            const std::map<std::string, ParameterBindingPtr>& inArgs,
            const std::map<std::string, ParameterBindingPtr>& outArgs
            ) = 0;


    protected:


        template <typename T>
        static inline const typename T::type& getInput(const std::map<std::string, ParameterBindingPtr>& inputArgs)
        {
            auto it = inputArgs.find(T::name());

            if (inputArgs.end() == it)
            {
                throw ArgumentException(T::name(), "Cannot find parameter");
            }

            auto binding = it->second;

            auto * bind = dynamic_cast<const TypedBinding<typename T::type>*>(it->second.get());
            if (bind == nullptr)
            {
                throw ArgumentTypeMismatchException(T::name(), binding->type(), TypedBinding<typename T::type>::static_type());
            }

            return bind->get();
        }

        template <typename T>
        static inline typename T::type& getOutput(const std::map<std::string, ParameterBindingPtr>& outputArgs)
        {
            auto it = outputArgs.find(T::name());

            if (outputArgs.end() == it)
            {
                throw ArgumentException(T::name(), "Cannot find parameter");
            }

            auto binding = it->second;

            auto * bind = dynamic_cast<TypedBinding<typename T::type>*>(it->second.get());
            if (bind == nullptr)
                throw ArgumentTypeMismatchException(T::name(), binding->type(), TypedBinding<typename T::type>::static_type());

            return bind->get();
        }
    };

    typedef std::shared_ptr<Algorithm> AlgorithmPtr;

    void ProcessAlgorithm(AlgorithmInfoPtr algorithm, v8::Local<v8::Object> args, v8::Local<v8::Function> resultsCallback);

}