/**********************************************************************************
* CloudCV Boostrap - A starter template for Node.js with OpenCV bindings.
*                    This project lets you to quickly prototype a REST API
*                    in a Node.js for a image processing service written in C++.
*
* Author: Eugene Khvedchenya <ekhvedchenya@gmail.com>
*
* More information:
*  - https://cloudcv.io
*  - http://computer-vision-talks.com
*
**********************************************************************************/
#include "framework/NanCheck.hpp"
#include "framework/Logger.hpp"

namespace cloudcv
{

    ArgumentMismatchException::ArgumentMismatchException(const std::string& msg)
        : mMessage(msg)
    {
        LOG_TRACE_MESSAGE(mMessage);
    }

    ArgumentMismatchException::ArgumentMismatchException(int actual, int expected)
        : mMessage("Invalid number of arguments passed to a function")
    {
        LOG_TRACE_MESSAGE(mMessage);
    }

    ArgumentMismatchException::ArgumentMismatchException(int actual, const std::initializer_list<int>& expected)
        : mMessage("Invalid number of arguments passed to a function")
    {
        LOG_TRACE_MESSAGE(mMessage);
    }

    typedef std::function<bool(_NAN_METHOD_ARGS_TYPE) > InitFunction;

    class NanMethodArgBinding;
    class NanCheckArguments;

    NanMethodArgBinding::NanMethodArgBinding(int index, NanCheckArguments& parent)
        : mArgIndex(index)
        , mParent(parent)
    {
    }

    NanMethodArgBinding& NanMethodArgBinding::IsBuffer()
    {
        auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
        {
            bool isBuf = node::Buffer::HasInstance(args[mArgIndex]);
            LOG_TRACE_MESSAGE("Checking whether argument is function:" << isBuf);

            if (!isBuf)
                throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsBuffer check");
            return true;
        };

        mParent.AddAndClause(bind);
        return *this;
    }

    NanMethodArgBinding& NanMethodArgBinding::IsFunction()
    {
        auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
        {
            bool isFn = args[mArgIndex]->IsFunction();
            LOG_TRACE_MESSAGE("Checking whether argument is function:" << isFn);

            if (!isFn)
                throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsFunction check");

            return true;
        };
        mParent.AddAndClause(bind);
        return *this;
    }

    NanMethodArgBinding& NanMethodArgBinding::IsArray()
    {
        auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
        {
            bool isArr = args[mArgIndex]->IsArray();
            LOG_TRACE_MESSAGE("Checking whether argument is array:" << isArr);
            if (!isArr)
                throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsArray check");

            return true;
        };
        mParent.AddAndClause(bind);
        return *this;
    }

    NanMethodArgBinding& NanMethodArgBinding::IsObject()
    {
        auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
        {
            bool isArr = args[mArgIndex]->IsObject();
            LOG_TRACE_MESSAGE("Checking whether argument is object:" << isArr);
            if (!isArr)
                throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsObject check");

            return true;
        };
        mParent.AddAndClause(bind);
        return *this;
    }

    NanMethodArgBinding& NanMethodArgBinding::IsString()
    {
        auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
        {
            bool isStr = args[mArgIndex]->IsString() || args[mArgIndex]->IsStringObject();
            LOG_TRACE_MESSAGE("Checking whether argument is string:" << isStr);

            if (!isStr)
                throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsString check");

            return true;
        };
        mParent.AddAndClause(bind);
        return *this;
    }

    NanMethodArgBinding& NanMethodArgBinding::NotNull()
    {
        auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
        {
            if (args[mArgIndex]->IsNull())
            {
                throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates NotNull check");
            }
            return true;
        };
        mParent.AddAndClause(bind);
        return *this;
    }

    NanCheckArguments::NanCheckArguments(_NAN_METHOD_ARGS_TYPE args)
        : m_args(args)
        , m_init([](_NAN_METHOD_ARGS_TYPE args) { return true; })
        , m_error(0)
    {
    }


    NanCheckArguments& NanCheckArguments::ArgumentsCount(int count)
    {
        return AddAndClause([count](_NAN_METHOD_ARGS_TYPE args)
        {
            if (args.Length() != count)
                throw ArgumentMismatchException(args.Length(), count);

            return true;
        });
    }

    NanCheckArguments& NanCheckArguments::ArgumentsCount(int argsCount1, int argsCount2)
    {
        return AddAndClause([argsCount1, argsCount2](_NAN_METHOD_ARGS_TYPE args)
        {
            if (args.Length() != argsCount1 || args.Length() != argsCount2)
                throw ArgumentMismatchException(args.Length(), { argsCount1, argsCount2 });

            return true;
        });
    }

    NanMethodArgBinding NanCheckArguments::Argument(int index)
    {
        TRACE_FUNCTION;
        return NanMethodArgBinding(index, *this);
    }

    NanCheckArguments& NanCheckArguments::Error(std::string * error)
    {
        TRACE_FUNCTION;
        m_error = error;
        return *this;
    }

    /**
     * Unwind all fluent calls
     */
    NanCheckArguments::operator bool() const
    {
        TRACE_FUNCTION;
        try
        {
            return m_init(m_args);
        }
        catch (ArgumentMismatchException& exc)
        {
            LOG_TRACE_MESSAGE(exc.what());
            if (m_error)
            {
                *m_error = exc.what();
            }
            return false;
        }
        catch (...)
        {
            LOG_TRACE_MESSAGE("Caught unexpected exception");
            if (m_error)
            {
                *m_error = "Unknown error";
            }
            return false;
        }
    }

    NanCheckArguments& NanCheckArguments::AddAndClause(InitFunction rightCondition)
    {
        InitFunction prevInit = m_init;
        InitFunction newInit = [prevInit, rightCondition](_NAN_METHOD_ARGS_TYPE args) {
            return prevInit(args) && rightCondition(args);
        };
        m_init = newInit;
        return *this;
    }


    NanCheckArguments NanCheck(_NAN_METHOD_ARGS_TYPE args)
    {
        return std::move(NanCheckArguments(args));
    }
}