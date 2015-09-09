#pragma once

#include <memory>
#include <vector>
#include <v8.h>
#include <opencv2/opencv.hpp>

namespace cloudcv
{
    class AlgorithmParam
    {
    public:

        virtual ~AlgorithmParam() = default;
        virtual std::string name() const = 0;
        virtual std::string type() const = 0;
        
        virtual bool hasDefaultValue() const = 0;
    };
    
    template<typename T>
    class TypedParameter : public AlgorithmParam
    {
    public:
        TypedParameter(const char * name) 
            : m_name(name)
            , m_type(typeid(T).name())
            , m_hasDefaultValue(false)
        {
        }

        TypedParameter(const char * name, T defaultValue) 
            : m_name(name)
            , m_type(typeid(T).name())
            , m_hasDefaultValue(true)
            , m_default(defaultValue)
        {
        }
    
        std::string AlgorithmParam::name() const override
        {
            return m_name;
        }

        std::string AlgorithmParam::type() const override
        {
        }

        bool AlgorithmParam::hasDefaultValue() const override
        {
            return m_hasDefaultValue;
        }

        static std::shared_ptr<AlgorithmParam> create(const char * name) { return std::shared_ptr<AlgorithmParam>(new TypedParameter<T>(name)); }

    private:
        std::string m_name;
        std::string m_type;
        bool        m_hasDefaultValue;
        T           m_default;
    };

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
    };

    struct no_deleter {
        template<typename T>
        void operator()(T * v) {}
    };

    template<typename T>
    class TypedBinding : public ParameterBinding
    {
    public:
        TypedBinding(const char * name)
            : m_name(name)
            , m_type(typeid(T).name())
        {
        }

        TypedBinding(const char * name, const T& value)
            : m_name(name)
            , m_type(typeid(T).name())
            , m_value(&value, no_deleter())
        {
        }

        virtual std::string name() const {
            return m_name;
        }

        virtual std::string type() const {
            return m_type;
        }


        const T& get() const
        {
            return *m_value.get();
        }

        T& get()
        {
            return *m_value.get();
        }

    private:
        std::string        m_name;
        std::string        m_type;
        std::shared_ptr<T> m_value;
    };

    typedef std::shared_ptr<ParameterBinding> ParameterBindingPtr;

    class InputParameter
    {
    public:
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, bool value);
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, int value);
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, double value);
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, cv::Mat value);

        // Untyped bind
        static std::shared_ptr<ParameterBinding> Bind(AlgorithmParamPtr key, v8::Local<v8::Value> value);
    };

    class OutputParameter
    {
    public:
        static std::shared_ptr<ParameterBinding> Create(AlgorithmParamPtr key);
    };

    class Algorithm
    {
    public:
        virtual ~Algorithm() = default;
        virtual AlgorithmInfoPtr info() = 0;
        virtual void process(const std::vector<ParameterBindingPtr>& inputArgs, std::vector<ParameterBindingPtr>& outputArgs) = 0;

        static std::shared_ptr<Algorithm> create(std::string name);
    };

    typedef std::shared_ptr<Algorithm> AlgorithmPtr;

    void ProcessAlgorithm(AlgorithmPtr algorithm, const v8::FunctionCallbackInfo<v8::Value>& args);

}