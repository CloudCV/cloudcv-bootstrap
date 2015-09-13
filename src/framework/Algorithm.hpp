#pragma once

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>
#include <map>
#include <node.h>
#include <v8.h>
#include <nan.h>

#include "framework/ImageSource.hpp"
#include "framework/marshal/marshal.hpp"
#include "framework/marshal/opencv.hpp"

namespace cloudcv
{
    class ArgumentException : public std::runtime_error
    {
    public:
        inline ArgumentException(std::string message) : std::runtime_error(message)
        {

        }
    };

    class MissingInputArgumentException : public ArgumentException
    {
    public:
        inline MissingInputArgumentException(std::string argumentName)
            : ArgumentException("Missing argument " + argumentName)
        {

        }
    };

    class ArgumentTypeMismatchException : public ArgumentException
    {
    public:
        inline ArgumentTypeMismatchException(std::string argumentName, std::string actualType, std::string expectedType)
            : ArgumentException("Incompatible type for " + argumentName + " expected " + expectedType + " got " + actualType)
        {

        }
    };

    

    class AlgorithmParamVisitor;
    class ParameterBinding;

    class AlgorithmParam
    {
    public:

        virtual bool visit(AlgorithmParamVisitor * visitor) = 0;

        virtual ~AlgorithmParam() {};
        virtual std::string name() const = 0;
        virtual std::string type() const = 0;

        virtual std::shared_ptr<ParameterBinding> createDefault() = 0;
        virtual bool hasDefaultValue() const = 0;
    };
    


    template<typename T>
    class TypedParameter : public AlgorithmParam
    {
    public:
        inline TypedParameter(const char * name)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_hasDefaultValue(false)
        {
        }

        inline TypedParameter(const char * name, T defaultValue)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_hasDefaultValue(true)
        {
            m_default = [defaultValue]() { return defaultValue; };
        }

        inline TypedParameter(const char * name, std::function<T()> defaultValue)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_hasDefaultValue(true)
            , m_default(defaultValue)
        {
        }

        inline std::string name() const override
        {
            return m_name;
        }

        inline std::string type() const override
        {
            return m_type;
        }

        inline bool hasDefaultValue() const override
        {
            return m_hasDefaultValue;
        }

        bool visit(AlgorithmParamVisitor * visitor) override;

        std::shared_ptr<ParameterBinding> createDefault() override;

    private:
        std::string m_name;
        std::string m_type;
        bool        m_hasDefaultValue;
        std::function<T()> m_default;
    };

    template<typename T>
    class OutputParameter : public TypedParameter<T>
    {
    public:
        inline OutputParameter(const char * name)
            : TypedParameter<T>(name, T())
        {
        }
    };

    class AlgorithmParamVisitor
    {
    public:
        bool apply(AlgorithmParam* parameter) { return false; }

        virtual bool apply(TypedParameter<bool>* parameter) { return false; };
        virtual bool apply(TypedParameter<int>* parameter) { return false; };
        virtual bool apply(TypedParameter<float>* parameter) { return false; };
        virtual bool apply(TypedParameter<double>* parameter) { return false; };
        virtual bool apply(TypedParameter<ImageSource>* parameter) { return false; };
        virtual bool apply(TypedParameter<std::vector<cv::Point2f>>* parameter) { return false; };
    };

    template<typename T>
    inline bool TypedParameter<T>::visit(AlgorithmParamVisitor * visitor) 
    {
        return visitor->apply(this);
    }

    typedef std::shared_ptr<AlgorithmParam> AlgorithmParamPtr;

    class AlgorithmInfo
    {
    public:
        virtual std::string name() const = 0;

        //virtual uint32_t constructorArguments() const = 0;
        virtual uint32_t inputArguments() const = 0;
        virtual uint32_t outputArguments() const = 0;

        //virtual ArgumentTypePtr getConstructorArgumentType(uint32_t argumentIndex) const = 0;
        virtual AlgorithmParamPtr getInputArgumentType(uint32_t argumentIndex) const = 0;
        virtual AlgorithmParamPtr getOutputArgumentType(uint32_t argumentIndex) const = 0;

        virtual ~AlgorithmInfo() = default;
    private:
    };

    typedef std::shared_ptr<AlgorithmInfo> AlgorithmInfoPtr;

    class ParameterBinding
    {
    public:
        virtual std::string name() const = 0;
        virtual std::string type() const = 0;

        virtual ~ParameterBinding() = default;

        virtual v8::Local<v8::Value> marshalFromNative() const = 0;
    };

    struct no_deleter {
        template<typename T>
        inline void operator()(T * v) {}
    };

    template<typename T>
    class TypedBinding : public ParameterBinding
    {
    public:
        inline TypedBinding(const std::string& name)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_value(nullptr)
        {
        }

        inline TypedBinding(const std::string& name, const T& value)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_value(const_cast<T*>(&value))
        {
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
            assert(m_value != nullptr);
            return *m_value;
        }

        inline T& get()
        {
            assert(m_value != nullptr);
            return *m_value;
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
        T*                 m_value;
    };

    typedef std::shared_ptr<ParameterBinding> ParameterBindingPtr;

    template <typename T>
    inline std::shared_ptr<ParameterBinding> TypedParameter<T>::createDefault()
    {
        return std::shared_ptr<ParameterBinding>(new TypedBinding<T>(m_name, m_default()));
    }


    class InputParameter
    {
    public:
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, bool value);
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, int value);
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, double value);
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, cv::Mat value);

        // Untyped bind
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, const v8::Local<v8::Value>& value);
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