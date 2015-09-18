#include "framework/AlgorithmExceptions.hpp"

namespace cloudcv
{
    ArgumentException::ArgumentException(const std::string& argumentName, const std::string& message)
        : std::runtime_error(argumentName + ":" + message)
    {
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