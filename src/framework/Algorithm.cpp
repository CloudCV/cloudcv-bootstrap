#include "framework/Algorithm.hpp"
#include "framework/Logger.hpp"
#include "framework/ScopedTimer.hpp"
#include "framework/Job.hpp"
#include "framework/marshal/marshal.hpp"
#include "framework/NanCheck.hpp"

#include <node.h>
#include <v8.h>
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
            Nan::Callback * callback)
            : Job(callback)
            , m_algorithm(alg)
            , m_input(inArgs)
            , m_output(outArgs)
        {
            TRACE_FUNCTION;
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
            try
            {
                TRACE_FUNCTION;
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
            TRACE_FUNCTION;
            

            Nan::EscapableHandleScope scope;

            v8::Local<v8::Object> outputArgument = Nan::New<v8::Object>();

            for (const auto& arg : m_output)
            {
                outputArgument->Set(marshal(arg.first), arg.second->marshalFromNative());
            }

            return scope.Escape(outputArgument);
        }
    };

    void ProcessAlgorithm(AlgorithmPtr algorithm, Nan::NAN_METHOD_ARGS_TYPE args)
    {
        TRACE_FUNCTION;
        
        using namespace cloudcv;

        TRACE_FUNCTION;
        Nan::HandleScope scope;

        v8::Local<v8::Object>   inputArguments;
        v8::Local<v8::Function> resultsCallback;
        std::string     error;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(2)
            .Argument(0).IsObject().Bind(inputArguments)
            .Argument(1).IsFunction().Bind(resultsCallback))
        {
            try
            {
                auto info = algorithm->info();
                std::map<std::string, ParameterBindingPtr> inArgs, outArgs;

                for (auto arg : info->inputArguments())
                {
                    auto propertyName = marshal(arg.first);
                    v8::Local<v8::Value> argumentValue = Nan::Null();

                    if (inputArguments->HasRealNamedProperty(propertyName->ToString()))
                        argumentValue = inputArguments->Get(propertyName);

                    LOG_TRACE_MESSAGE("Binding input argument " << arg.first);

                    auto bind = InputParameter::Bind(arg.second, argumentValue);
                    inArgs.insert(std::make_pair(arg.first, bind));
                }

                for (auto arg : info->outputArguments())
                {
                    LOG_TRACE_MESSAGE("Binding output argument " << arg.first);

                    auto bind = arg.second->createDefault();
                    outArgs.insert(std::make_pair(arg.first, bind));
                }

                Nan::Callback *callback = new Nan::Callback(resultsCallback);
                Nan::AsyncQueueWorker(new AlgorithmTask(algorithm, inArgs, outArgs, callback));
            }
            catch (cv::Exception& er)
            {
                LOG_TRACE_MESSAGE(er.what());
                Nan::ThrowError(er.what());
            }
            catch (std::runtime_error& er)
            {
                LOG_TRACE_MESSAGE(er.what());
                Nan::ThrowError(er.what());
            }

        }
        else if (!error.empty())
        {
            LOG_TRACE_MESSAGE("Cannot parse input arguments: " << error.c_str());
            Nan::ThrowTypeError(error.c_str());
        }
    }


    class InputArgumentBinder : public AlgorithmParamVisitor
    {
    private:
        const v8::Local<v8::Value>&       m_value;
        std::shared_ptr<ParameterBinding> m_bind;

    protected:

        template <typename T>
        inline bool convert(RangedArgument<T>* parameter)
        {
            if (m_value->IsUndefined() || m_value->IsNull())
            {
                if (parameter->hasDefaultValue())
                    m_bind = parameter->createDefault();
                else
                    throw MissingInputArgumentException(parameter->name());
            }
            else
            {
                m_bind.reset(new TypedBinding<T>(parameter->name(), marshal<T>(m_value)));
            }

            return true;
        }

    public:
        InputArgumentBinder(const v8::Local<v8::Value>& value)
            : m_value(value)
        {
        }

        std::shared_ptr<ParameterBinding> getBind()
        {
            return m_bind;
        }

        virtual bool apply(RangedArgument<bool>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(RangedArgument<ImageSource>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(RangedArgument<double>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(RangedArgument<float>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(RangedArgument<int>* parameter) override
        {
            return convert(parameter);
        }

        virtual bool apply(RangedArgument<std::vector<cv::Point2f>>* parameter) override
        {
            return convert(parameter);
        }
    };


    std::shared_ptr<ParameterBinding> InputParameter::Bind(InputArgumentPtr argument, const v8::Local<v8::Value>& value)
    {
        InputArgumentBinder visitor(value);
        if (argument->visit(&visitor))
        {
            return visitor.getBind();
        }

        throw std::runtime_error("Cannot detect type of input parameter " + argument->name());
    }

}