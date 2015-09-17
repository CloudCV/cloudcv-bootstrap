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
    class ParameterBinding;

    class Argument
    {
    public:
        virtual ~Argument() = default;
        
        const std::string& name() const;
        const std::string& type() const;

    protected:
        Argument(const std::string& name, const std::string& type);

    private:
        std::string m_name;
        std::string m_type;
    };

    class InputArgument : public Argument
    {
    public:
        virtual ~InputArgument() = default;
    protected:
        InputArgument(const std::string& name, const std::string& type);
    };

    class OutputArgument : public Argument
    {
    public:
        virtual ~OutputArgument() = default;

        template <typename T>
        static std::shared_ptr<OutputArgument> Create(const char * name);

    protected:
        OutputArgument(const std::string& name, const std::string& type);
    };

    template <typename ArgumentBase>
    class AlgorithmParamVisitor;

    template<typename T, typename ArgumentBase>
    class TypedArgument : public ArgumentBase
    {
    public:
        static std::shared_ptr<InputArgument> Create(const char * name)
        {
            return std::shared_ptr<InputArgument>(new RequiredArgument<T>(name));
        }

        std::string name() const override
        {
            return m_name;
        }

        std::string type() const override
        {
            return m_type;
        }

        bool visit(AlgorithmParamVisitor<ArgumentBase> * visitor) override
        {
            return visitor->apply(this);
        }

    protected:
        TypedArgument(const char * name)
            : ArgumentBase(name, typeid(T).name())
        {
        }
    };

    template <typename ArgumentBase>
    class AlgorithmParamVisitor
    {
    public:
        template <typename T>
        bool accept(TypedArgument<T, ArgumentBase> * argument);
    };


    template <typename T>
    std::shared_ptr<OutputArgument> OutputArgument::Create(const char * name)
    {
        return std::shared_ptr<OutputArgument>(new TypedArgument<T, OutputArgument>(name));        
    }

    template<typename T>
    class RangedArgument : public TypedArgument<T, InputArgument>
    {
    public:
        static std::shared_ptr<InputArgument> Create(const char * name, T minValue, T defaultValue, T maxValue)
        {
            return std::shared_ptr<InputArgument>(new RangedArgument<T>(name, minValue, defaultValue, maxValue));
        }

        bool hasDefaultValue() const override
        {
            return true;
        }

        std::shared_ptr<ParameterBinding> createDefault() override;

    protected:
        RangedArgument(const char * name, T minValue, T defaultValue, T maxValue)
            : TypedArgument(name, typeid(T).name())
            , m_min(minValue)
            , m_max(maxValue)
            , m_default(defaultValue)
        {
            if (minValue >= defaultValue)
                throw ArgumentException();
        }

    private:
        T           m_min;
        T           m_max;
        T           m_default;
    };
}