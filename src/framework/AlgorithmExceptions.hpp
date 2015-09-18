#pragma once

#include <string>
#include <memory>
#include <stdexcept>

namespace cloudcv
{
    class ArgumentException : public std::runtime_error
    {
    public:
        ArgumentException(const std::string& message);
    };

    class MissingInputArgumentException : public ArgumentException
    {
    public:
        MissingInputArgumentException(std::string argumentName);
    };

    class ArgumentTypeMismatchException : public ArgumentException
    {
    public:
        ArgumentTypeMismatchException(
            std::string argumentName, 
            std::string actualType, 
            std::string expectedType);
    };

    class ArgumentBindException : public ArgumentException
    {
    public:
        ArgumentBindException(std::string argumentName, std::string actualType, std::string expectedType);
    };
}