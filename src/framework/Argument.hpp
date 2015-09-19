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

#pragma once

#include <memory>

namespace cloudcv
{
    class InputArgument;
    class OutputArgument;
    class ParameterBinding;

    class ParameterBinding
    {
    public:
        virtual std::string type() const = 0;

        virtual ~ParameterBinding() = default;

        virtual v8::Local<v8::Value> marshalFromNative() const = 0;
    };

    template <class T>
    class TypedBinding : public ParameterBinding
    {
    public:
        TypedBinding(T value)
            : m_value(value)
        {            
        }

        TypedBinding()
        {
        }

        static std::string static_type()  { return typeid(T).name(); }

        std::string type() const { return static_type(); }

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
        T           m_value;
    };


    template<typename T>
    std::shared_ptr<ParameterBinding> wrap_as_bind(const T& value)
    {
        return std::shared_ptr<ParameterBinding>(new TypedBinding<T>(value));
    }


    class InputArgument
    {
    public:
        virtual ~InputArgument() = default;

        virtual std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) = 0;

        const std::string& name() const { return m_name; }
        const std::string& type() const { return m_type; }

    protected:
        InputArgument(const std::string& name, const std::string& type)
        {
        }

    private:
        std::string m_name;
        std::string m_type;
    };

    typedef std::shared_ptr<InputArgument> InputArgumentPtr;

    class OutputArgument
    {
    public:

        virtual ~OutputArgument() = default;

        virtual std::shared_ptr<ParameterBinding> bind() = 0; 

        const std::string& name() const { return m_name; }
        const std::string& type() const { return m_type; }

    protected:
        OutputArgument(const std::string& name, const std::string& type)
            : m_name(name)
            , m_type(type)
        {
        }

    private:
        std::string m_name;
        std::string m_type;
    };

    typedef std::shared_ptr<OutputArgument> OutputArgumentPtr;

    template <typename T>
    class TypedOutputArgument : public OutputArgument
    {
    public:
        static std::pair<std::string, OutputArgumentPtr>  Create(const char * name)
        {
            return std::make_pair(name, std::shared_ptr<OutputArgument>(new TypedOutputArgument<T>(name)));
        }

        std::shared_ptr<ParameterBinding> bind() override
        {
            return wrap_as_bind(T());
        }

    protected:
        TypedOutputArgument(const char * name)
            : OutputArgument(name, typeid(T).name())
        {
        }
    };



    template <typename T>
    class RequiredArgument : public InputArgument
    {
    public:
        static inline std::pair<std::string, InputArgumentPtr> Create(const char * name)
        {
            return std::make_pair(name, std::shared_ptr<InputArgument>(new RequiredArgument<T>(name)));
        }

        std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) override
        {
            if (value->IsUndefined() || value->IsNull())
            {
                throw ArgumentBindException(name(), "Missing required argument");
            }

            return wrap_as_bind(marshal<T>(value));
        }

    protected:
        inline RequiredArgument(const char * name)
            : InputArgument(name, typeid(T).name())
        {
        }
    };


    template <typename T>
    class RangedArgument : public InputArgument
    {
    public:
        static inline  std::pair<std::string, InputArgumentPtr> Create(const char * name, T minValue, T defaultValue, T maxValue)
        {
            return std::make_pair(name, std::shared_ptr<InputArgument>(new RangedArgument<T>(name, minValue, defaultValue, maxValue)));
        }

        std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) override
        {
            if (value->IsUndefined() || value->IsNull())
            {
                return wrap_as_bind(m_default);
            }

            return wrap_as_bind(validate(marshal<T>(value)));
        }

    protected:
        inline RangedArgument(const char * name, T minValue, T defaultValue, T maxValue)
            : InputArgument(name, typeid(T).name())
            , m_min(minValue)
            , m_max(maxValue)
            , m_default(defaultValue)
        {
            m_default = validate(m_default);
        }

        inline T validate(T value) const
        {
            if (value < m_min)
                throw ArgumentBindException(name(), "Value cannot be less than " + std::to_string(m_min));

            if (value > m_max)
                throw ArgumentBindException(name(), "Value cannot be greater than " + std::to_string(m_max));

            return value;
        }
    private:
        T           m_min;
        T           m_max;
        T           m_default;
    };
}