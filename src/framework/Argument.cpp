#include "framework/Argument.hpp"

namespace cloudcv
{

    Argument::Argument(const std::string& name, const std::string& type)
        : m_name(name)
        , m_type(type)
    {
    }

    const std::string& Argument::name() const
    {
        return m_name;
    }

    const std::string& Argument::type() const
    {
        return m_type;
    }

    InputArgument::InputArgument(const std::string& name, const std::string& type)
        : Argument(name, type)
    {
    }


    OutputArgument::OutputArgument(const std::string& name, const std::string& type)
        : Argument(name, type)
    {
    }

}