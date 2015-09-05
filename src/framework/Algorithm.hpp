#pragma once

namespace cloudcv
{
    enum ArgumentType 
    {
        ArgumentTypeBoolean,
        ArgumentTypeInteger,
        ArgumentTypeFloat,
        ArgumentTypeString,
        ArgumentTypeMatrix
    };

    class AlgorithmParam
    {
    public:

        virtual ~AlgorithmParam() = default;
        virtual std::string name() const = 0;
        virtual ArgumentType type() const = 0;
        
        virtual bool hasDefaultValue() const = 0;
    };
    

    typedef std::shared_ptr<AlgorithmParam> AlgorithmParamPtr;

    class AlgorithmInfo
    {
    public:
        virtual std::string name() const = 0;

        //virtual uint32_t constructorArguments() const = 0;
        virtual uint32_t inputArguments() const = 0;
        virtual uint32_t outputArguments() const = 0;

        //virtual ArgumentTypePtr getConstructorArgumentType(uint32_t argumentIndex) const = 0;
        virtual AlgorithmParamPtr getInputArgumentType(uint32_t argumentIndex) const = 0;
        virtual AlgorithmParamPtr getOutputArgumentType(uint32_t argumentIndex) const = 0;
        
        virtual ~AlgorithmInfo() = default;
    private:
    };

    typedef std::shared_ptr<AlgorithmInfo> AlgorithmInfoPtr;

    class ParameterBinding
    {
    public:
        virtual std::string name() const = 0;
        virtual ArgumentType type() const = 0;
        virtual bool hasDefaultValue() const = 0;

        virtual ~ParameterBinding() = default;
    };

    class Algorithm
    {
    public:
        virtual void process(const std::vector<ParameterBinding>& inputArgs, const std::vector<ParameterBinding>& outputArgs) = 0;

        static std::shared_ptr<Algorithm> create(std::string name);
    };
}