#pragma once

#include <memory>
#include <stdexcept>

namespace cloudcv
{
    class ArgumentException : public std::runtime_error
    {
    public:
        inline ArgumentException(const std::string& message) : std::runtime_error(message)
        {
            int d = 0;
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

    class ArgumentBindException : public ArgumentException
    {
    public:
        inline ArgumentBindException(std::string argumentName, std::string actualType, std::string expectedType)
            : ArgumentException("ArgumentBindException for argument " + argumentName)
        {

        }
    };
}