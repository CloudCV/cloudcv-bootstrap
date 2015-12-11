#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include "framework/CompilerSupport.hpp"

namespace cloudcv
{
    class ArgumentException : public std::runtime_error
    {
    public:
        ArgumentException(const std::string& argumentName, const std::string& message);

        const std::string& argument() const CLOUDCV_NOTHROW;
        
        const char * what() const CLOUDCV_NOTHROW override;

    private:
        std::string m_argumentName;
        std::string m_message;
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
        ArgumentBindException(std::string argumentName, std::string message);
    };
}