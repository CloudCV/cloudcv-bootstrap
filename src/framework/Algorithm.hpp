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

    class AlgorithmParamVisitor
    {
    public:
        bool apply(Argument* parameter) { return false; }

        virtual bool apply(RangedArgument<bool>* parameter) { return false; };
        virtual bool apply(RangedArgument<int>* parameter) { return false; };
        virtual bool apply(RangedArgument<float>* parameter) { return false; };
        virtual bool apply(RangedArgument<double>* parameter) { return false; };
        virtual bool apply(RangedArgument<ImageSource>* parameter) { return false; };
        virtual bool apply(RangedArgument<std::vector<cv::Point2f>>* parameter) { return false; };
    };

    template<typename T>
    inline bool RangedArgument<T>::visit(AlgorithmParamVisitor * visitor)
    {
        return visitor->apply(this);
    }





    class ParameterBinding
    {
    public:
        virtual std::string name() const = 0;
        virtual std::string type() const = 0;

        virtual ~ParameterBinding() = default;

        virtual v8::Local<v8::Value> marshalFromNative() const = 0;
    };


    template<typename T>
    class TypedBinding : public ParameterBinding
    {
    public:

        inline TypedBinding(const std::string& name, T value)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_value(value)
        {
            LOG_TRACE_MESSAGE("Created binding to parameter " + name);
        }

        inline static std::string static_name() {
            return typeid(T).name();
        }

        virtual std::string name() const override {
            return m_name;
        }

        virtual std::string type() const override {
            return m_type;
        }


        inline const T& get() const
        {
            return m_value;
        }

        inline T& get()
        {
            return m_value;
        }

        inline v8::Local<v8::Value> marshalFromNative() const override
        {
            Nan::EscapableHandleScope scope;
            const T& val = get();
            return scope.Escape(marshal(val));
        }

    private:
        std::string        m_name;
        std::string        m_type;
        T                  m_value;
    };

    typedef std::shared_ptr<ParameterBinding> ParameterBindingPtr;

    template <typename T>
    inline std::shared_ptr<ParameterBinding> RangedArgument<T>::createDefault()
    {
        return std::shared_ptr<ParameterBinding>(new TypedBinding<T>(m_name, m_default));
    }


    class InputParameter
    {
    public:
        static std::shared_ptr<ParameterBinding> Bind(InputArgumentPtr key, const v8::Local<v8::Value>& value);
    };

    class OutputParameter
    {
    public:
        static std::shared_ptr<ParameterBinding> Bind(OutputArgumentPtr key);
    };

    class Algorithm
    {
    public:
        virtual ~Algorithm() = default;
        virtual AlgorithmInfoPtr info() = 0;
        virtual void process(
            const std::map<std::string, ParameterBindingPtr>& inArgs,
            const std::map<std::string, ParameterBindingPtr>& outArgs
            ) = 0;


    protected:
        template <typename T>
        static inline  const T& getInput(const std::map<std::string, ParameterBindingPtr>& inputArgs,
            const std::string& name)
        {
            auto it = inputArgs.find(name);

            if (inputArgs.end() == it) {
                throw ArgumentException("Cannot find parameter " + name);
                // TODO: Check for null
            }

            auto binding = it->second;

            auto * bind = dynamic_cast<const TypedBinding<T>*>(it->second.get());
            if (bind == nullptr)
                throw ArgumentTypeMismatchException(binding->name(), binding->type(), TypedBinding<T>::static_name());

            return bind->get();
        }

        template <typename T>
        static inline  T& getOutput(const std::map<std::string, ParameterBindingPtr>& outputArgs,
            const std::string& name)
        {
            auto it = outputArgs.find(name);

            if (outputArgs.end() == it) {
                // TODO: Check for null
            }

            auto binding = it->second;

            auto * bind = dynamic_cast<TypedBinding<T>*>(it->second.get());
            if (bind == nullptr)
                throw ArgumentTypeMismatchException(binding->name(), binding->type(), TypedBinding<T>::static_name());

            return bind->get();
        }
    };

    typedef std::shared_ptr<Algorithm> AlgorithmPtr;

    void ProcessAlgorithm(AlgorithmPtr algorithm, Nan::NAN_METHOD_ARGS_TYPE args);

}