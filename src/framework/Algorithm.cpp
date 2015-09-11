#include "framework/Algorithm.hpp"
#include "framework/Logger.hpp"
#include "framework/ScopedTimer.hpp"
#include "framework/NanCheck.hpp"
#include "framework/Job.hpp"
#include "framework/marshal/marshal.hpp"
#include <nan.h>

namespace cloudcv
{


    V8Result marshal(ParameterBindingPtr val)
    {
        NanEscapableScope();
        return NanEscapeScope(val->marshalFromNative());
    }

    class AlgorithmTask : public Job
    {
        AlgorithmPtr                     m_algorithm;
        std::vector<ParameterBindingPtr> m_input;
        std::vector<ParameterBindingPtr> m_output;

    public:

        AlgorithmTask(
            AlgorithmPtr alg, 
            std::vector<ParameterBindingPtr> inArgs, 
            std::vector<ParameterBindingPtr> outArgs,  
            NanCallback * callback)
            : Job(callback)
            , m_algorithm(alg)
            , m_input(inArgs)
            , m_output(outArgs)
        {
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
                SetErrorMessage(err.what());
            }
            catch (cv::Exception& err)
            {
                SetErrorMessage(err.what());
            }
            catch (std::runtime_error& err)
            {
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
                outputArgument->Set(marshal(arg->name()), marshal(arg));
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

            std::vector<ParameterBindingPtr> inArgs, outArgs;

            for (size_t inArgIdx = 0; inArgIdx < info->inputArguments(); inArgIdx++)
            {
                auto arg = info->getInputArgumentType(inArgIdx);

                if (!arg->hasDefaultValue())
                {
                    Local<Value> argumentValue = inputArguments->Get(marshal(arg->name()));

                    auto bind = InputParameter::Bind(arg, argumentValue);
                    inArgs.push_back(bind);
                }
            }

            for (size_t outArgIdx = 0; outArgIdx < info->outputArguments(); outArgIdx++)
            {
                auto arg = info->getOutputArgumentType(outArgIdx);
                auto bind = OutputParameter::Create(arg);
                outArgs.push_back(bind);
            }

            NanCallback *nanCallback = new NanCallback(resultsCallback);
            NanAsyncQueueWorker(new AlgorithmTask(algorithm, inArgs, outArgs, nanCallback));

        }
        else if (!error.empty())
        {
            std::cout << "Cannot parse input arguments: " << error.c_str() << std::endl;
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