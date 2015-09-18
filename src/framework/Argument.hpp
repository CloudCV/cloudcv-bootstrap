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
        virtual std::string name() const = 0;
        virtual std::string type() const = 0;

        virtual ~ParameterBinding() = default;

        virtual v8::Local<v8::Value> marshalFromNative() const = 0;
    };

    template <class T>
    class TypedBinding : public ParameterBinding
    {
    public:
        TypedBinding(const char * name)
            : m_name(name)
        {            
        }

        std::string name() const { return m_name;           }
        std::string type() const { return typeid(T).name(); }

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
        std::string m_name;
        T           m_value;
    };

    class InputArgumentVisitor
    {
    public:
        virtual ~InputArgumentVisitor() = default;

        //template <typename T>
        //bool accept(InputArgument<T> * argument);
    };

    class OutputArgumentVisitor
    {
    public:
        virtual ~OutputArgumentVisitor() = default;

        //template <typename T>
        //bool accept(InputArgument<T> * argument);
    };

    class InputArgument
    {
    public:
        virtual ~InputArgument() = default;

        virtual bool visit(InputArgumentVisitor * visitor);

        virtual std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) = 0;

    protected:
        InputArgument(const std::string& name, const std::string& type)
        {
        }

    private:
        std::string m_name;
        std::string m_type;
    };

    class OutputArgument
    {
    public:

        virtual ~OutputArgument() = default;

        virtual bool visit(OutputArgumentVisitor * visitor);

        virtual std::shared_ptr<ParameterBinding> bind() = 0; 

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

    template <typename T>
    class TypedOutputArgument : public OutputArgument
    {
    public:
        std::shared_ptr<OutputArgument> Create(const char * name)
        {
            return std::shared_ptr<OutputArgument>(new TypedOutputArgument<T>(name));        
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
        static inline std::shared_ptr<InputArgument> Create(const char * name)
        {
            return std::shared_ptr<InputArgument>(new RequiredArgument<T>(name));
        }

        std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) override
        {
            return wrap(marshal<T>(value));
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
        static inline std::shared_ptr<InputArgument> Create(const char * name, T minValue, T defaultValue, T maxValue)
        {
            return std::shared_ptr<InputArgument>(new RangedArgument<T>(name, minValue, defaultValue, maxValue));
        }

        bool hasDefaultValue() const override
        {
            return true;
        }

        std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) override
        {
            return wrap(validate(marshal<T>(value)));
        }


    protected:
        inline RangedArgument(const char * name, T minValue, T defaultValue, T maxValue)
            : InputArgument(name, typeid(T).name())
            , m_min(minValue)
            , m_max(maxValue)
            , m_default(defaultValue)
        {
            if (minValue > defaultValue)
                throw ArgumentException();

            if (maxValue < defaultValue)
                throw ArgumentException();
        }

        inline T validate(T value) const
        {
            if (value < m_min)
                throw ArgumentBindException("Value cannot be less than " + std::to_string(m_min));

            if (value > m_max)
                throw ArgumentBindException("Value cannot be greater than " + std::to_string(m_max));

            return value;
        }
    private:
        T           m_min;
        T           m_max;
        T           m_default;
    };
}