#include "framework/AlgorithmExceptions.hpp"

namespace cloudcv
{
    ArgumentException::ArgumentException(const std::string& argumentName, const std::string& message)
        : std::runtime_error("ArgumentException")
        , m_argumentName(argumentName)
        , m_message(message)
    {
    }

    const std::string& ArgumentException::argument() const CLOUDCV_NOTHROW
    {
        return m_argumentName; 
    }

    const char * ArgumentException::what() const CLOUDCV_NOTHROW
    {
        return m_message.c_str();
    }


    MissingInputArgumentException::MissingInputArgumentException(std::string argumentName)
        : ArgumentException(argumentName, "Missing argument")
    {
    }

    ArgumentTypeMismatchException::ArgumentTypeMismatchException(std::string argumentName, std::string actualType, std::string expectedType)
        : ArgumentException(argumentName, "Expected type " + expectedType + " got: " + actualType)
    {
    }

    ArgumentBindException::ArgumentBindException(std::string argumentName, std::string message)
        : ArgumentException(argumentName, message)
    {
    }

}