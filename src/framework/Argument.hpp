#pragma once

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>
#include <map>
#include <node.h>
#include <v8.h>
#include <nan.h>
#include <nan-marshal.h>
#include <type_traits>

#include "framework/ImageView.hpp"
#include "framework/marshal/opencv.hpp"

#pragma once

#include <memory>


namespace cloudcv
{
    template<class T>
    struct is_fixed_array : std::false_type {
    };

    template<class T, std::size_t N>
    struct is_fixed_array<T[N]> : std::true_type { 
    };

    template<class T, std::size_t N>
    struct is_fixed_array< std::array<T,N> > : std::true_type {
    };

    template<class T>
    struct is_vector : std::false_type {
    };
     
    template<class T>
    struct is_vector< std::vector<T> > : std::true_type {
    };

    template<class T> struct underlying_type {
        typedef T value_type;
    };

    template<class T> struct underlying_type< std::vector<T> > {
        typedef T value_type;
    };

    template<class T, size_t N> struct underlying_type<T[N]> {
        typedef T value_type;
    };
     

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

        std::string type() const override { return static_type(); }

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
            return scope.Escape(Nan::Marshal(val));
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

        //! Serialize argument information
        virtual void serialize(Nan::marshal::SaveArchive& value) const = 0;

    protected:
        InputArgument(const std::string& name, const std::string& type)
            : m_name(name)
            , m_type(type)
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

        virtual void serialize(Nan::marshal::SaveArchive& value) const = 0;

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

        virtual std::shared_ptr<ParameterBinding> bind() override
        {
            return wrap_as_bind(T());
        }

        virtual void serialize(Nan::marshal::SaveArchive& value) const override
        {
            value & Nan::marshal::make_nvp("name", name());

            using namespace std;

            if (is_fixed_array<T>::value)
            {
                std::string elementType = typeid(typename underlying_type<T>::value_type).name();
                value & Nan::marshal::make_nvp("type", std::string("array"));
                //value & Nan::marshal::make_nvp("size", is_fixed_array<T>::size());
                value & Nan::marshal::make_nvp("elementType", elementType);
            }
            else if (is_vector<T>::value)
            {
                std::string elementType = typeid(typename underlying_type<T>::value_type).name();
                value & Nan::marshal::make_nvp("type", std::string("array"));
                value & Nan::marshal::make_nvp("elementType", elementType);
            }
            else
            {
                value & Nan::marshal::make_nvp("type", type());                
            }
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
                throw ArgumentBindException(name(), "Missing required argument \"" + name() + "\"");
            }

            return wrap_as_bind(Nan::Marshal<T>(value));
        }

        //! Serialize argument information
        virtual void serialize(Nan::marshal::SaveArchive& value) const override
        {
            value & Nan::marshal::make_nvp("name", name());
            value & Nan::marshal::make_nvp("type", type());
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
        static inline std::pair<std::string, InputArgumentPtr> Create(const char * name, T minValue, T defaultValue, T maxValue)
        {
            return std::make_pair(name, std::shared_ptr<InputArgument>(new RangedArgument<T>(name, minValue, defaultValue, maxValue)));
        }

        std::shared_ptr<ParameterBinding> bind(v8::Local<v8::Value> value) override
        {
            if (value->IsUndefined() || value->IsNull())
            {
                return wrap_as_bind(m_default);
            }

            return wrap_as_bind(validate(Nan::Marshal<T>(value)));
        }

        //! Serialize argument information
        virtual void serialize(Nan::marshal::SaveArchive& value) const override
        {
            using namespace Nan::marshal;

            value & make_nvp("name", name());
            value & make_nvp("type", type());

            value & make_nvp("min", m_min);
            value & make_nvp("max", m_max);
            value & make_nvp("default", m_default);
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

    template <typename T>
    static inline std::pair<std::string, InputArgumentPtr> inputArgument()
    {
        return RequiredArgument<typename T::type>::Create(T::name());
    }

    template <typename T>
    static inline std::pair<std::string, InputArgumentPtr> inputArgument(typename T::type minValue, typename T::type defaultValue, typename T::type maxValue)
    {
        return RangedArgument<typename T::type>::Create(T::name(), minValue, defaultValue, maxValue);
    }

    template <typename T>
    static inline std::pair<std::string, OutputArgumentPtr> outputArgument()
    {
        return TypedOutputArgument<typename T::type>::Create(T::name());
    }
}