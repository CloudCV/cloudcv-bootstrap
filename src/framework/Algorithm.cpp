#include "framework/Algorithm.hpp"
#include "framework/Logger.hpp"
#include "framework/ScopedTimer.hpp"
#include "framework/NanCheck.hpp"
#include "framework/Job.hpp"
#include "framework/marshal/marshal.hpp"
#include <nan.h>

namespace cloudcv
{
    class AlgorithmTask : public Job
    {
        AlgorithmPtr                               m_algorithm;
        std::map<std::string, ParameterBindingPtr> m_input;
        std::map<std::string, ParameterBindingPtr> m_output;

    public:

        AlgorithmTask(
            AlgorithmPtr alg, 
            std::map<std::string, ParameterBindingPtr> inArgs,
            std::map<std::string, ParameterBindingPtr> outArgs,
            NanCallback * callback)
            : Job(callback)
            , m_algorithm(alg)
            , m_input(inArgs)
            , m_output(outArgs)
        {
            LOG_TRACE_MESSAGE(alg->info()->name());
            LOG_TRACE_MESSAGE("Input arguments:" << inArgs.size());
            LOG_TRACE_MESSAGE("Output arguments:" << outArgs.size());
        }

    protected:

        // This function is executed in another thread at some point after it has been
        // scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
        // will crash randomly and you'll have a lot of fun debugging.
        // If you want to use parameters passed into the original call, you have to
        // convert them to PODs or some other fancy method.
        void ExecuteNativeCode() override
        {
            TRACE_FUNCTION;
            try
            {
                m_algorithm->process(m_input, m_output);
            }
            catch (ArgumentException& err)
            {
                LOG_TRACE_MESSAGE("ArgumentException:" << err.what());
                SetErrorMessage(err.what());
            }
            catch (cv::Exception& err)
            {
                LOG_TRACE_MESSAGE("cv::Exception:" << err.what());
                SetErrorMessage(err.what());
            }
            catch (std::runtime_error& err)
            {
                LOG_TRACE_MESSAGE("std::runtime_error:" << err.what());
                SetErrorMessage(err.what());
            }
        }

        // This function is executed in the main V8/JavaScript thread. That means it's
        // safe to use V8 functions again. Don't forget the HandleScope!
        v8::Local<v8::Value> CreateCallbackResult() override
        {
            using namespace v8;

            NanEscapableScope();

            Local<Object> outputArgument = NanNew<Object>();

            for (const auto& arg : m_output)
            {
                outputArgument->Set(marshal(arg->name()), arg->marshalFromNative());
            }

            return NanEscapeScope(outputArgument);
        }
    };

    void ProcessAlgorithm(AlgorithmPtr algorithm, const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        using namespace v8;
        using namespace cloudcv;

        TRACE_FUNCTION;
        NanEscapableScope();

        Local<Object>   inputArguments;
        Local<Function> resultsCallback;
        std::string     error;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(2)
            .Argument(0).IsObject().Bind(inputArguments)
            .Argument(1).IsFunction().Bind(resultsCallback))
        {
            auto info = algorithm->info();

            std::map<std::string,ParameterBindingPtr> inArgs, outArgs;

            for (size_t inArgIdx = 0; inArgIdx < info->inputArguments(); inArgIdx++)
            {
                auto arg = info->getInputArgumentType(inArgIdx);
                Local<Value> argumentValue = inputArguments->Get(marshal(arg->name()));
                auto bind = InputParameter::Bind(arg, argumentValue);
                inArgs.insert(std::make_pair(arg->name(), bind));
            }

            for (size_t outArgIdx = 0; outArgIdx < info->outputArguments(); outArgIdx++)
            {
                auto arg = info->getOutputArgumentType(outArgIdx);
                auto bind = OutputParameter::Create(arg);
                outArgs.insert(std::make_pair(arg->name(), bind));
            }

            NanCallback *nanCallback = new NanCallback(resultsCallback);
            NanAsyncQueueWorker(new AlgorithmTask(algorithm, inArgs, outArgs, nanCallback));

        }
        else if (!error.empty())
        {
            LOG_TRACE_MESSAGE("Cannot parse input arguments: " << error.c_str());
            NanThrowTypeError(error.c_str());
        }

        NanReturnUndefined();
    }


    class InputParameterBinder : public AlgorithmParamVisitor
    {
    private:
        v8::Local<v8::Value>              m_value;
        std::shared_ptr<ParameterBinding> m_bind;

    protected:

        template <typename T>
        bool convert(TypedParameter<T>* parameter)
        {
            if (m_value->IsUndefined())
            {
                if (parameter->hasDefaultValue())
                    m_bind = parameter->createDefault();
                else
                    throw MissingInputArgumentException(parameter->name());
            }

            m_bind.reset(new TypedBinding<T>(parameter->name(), marshal<T>(m_value)));
            return true;
        }

    public:
        InputParameterBinder(v8::Local<v8::Value> value)
        {
        }

        std::shared_ptr<ParameterBinding> getBind()
        {
            return m_bind;
        }

        virtual bool apply(TypedParameter<bool>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(TypedParameter<ImageSource>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(TypedParameter<double>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(TypedParameter<float>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(TypedParameter<int>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(TypedParameter<std::vector<cv::Point2f>>* parameter) override
        {
            return convert(parameter);
        }
    };


    std::shared_ptr<ParameterBinding> InputParameter::Bind(AlgorithmParamPtr key, v8::Local<v8::Value> value)
    {
        InputParameterBinder visitor(value);
        key->visit(&visitor);
        return visitor.getBind();
    }

    std::shared_ptr<ParameterBinding> OutputParameter::Create(AlgorithmParamPtr key)
    {
        return key->createDefault();
    }

}