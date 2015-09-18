#include "framework/AlgorithmExceptions.hpp"

namespace cloudcv
{
    ArgumentException::ArgumentException(const std::string& message) 
        : std::runtime_error(message)
    {
    }

    MissingInputArgumentException::MissingInputArgumentException(std::string argumentName)
        : ArgumentException("Missing argument " + argumentName)
    {
    }

    ArgumentTypeMismatchException::ArgumentTypeMismatchException(std::string argumentName, std::string actualType, std::string expectedType)
        : ArgumentException("Incompatible type for " + argumentName + " expected " + expectedType + " got " + actualType)
    {
    }

    ArgumentBindException::ArgumentBindException(std::string argumentName, std::string actualType, std::string expectedType)
        : ArgumentException("ArgumentBindException for argument " + argumentName)
    {
    }

}