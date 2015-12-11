#pragma once

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>
#include <map>
#include <node.h>
#include <v8.h>
#include <nan.h>

#include "framework/Argument.hpp"
#include "framework/marshal/marshal.hpp"

namespace cloudcv
{
    class Algorithm;
    class AlgorithmInfo;
    typedef std::shared_ptr<AlgorithmInfo> AlgorithmInfoPtr;

    class AlgorithmInfo
    {
    public:
        inline std::string name() const
        {
            return m_name;
        }

        inline const std::map<std::string, InputArgumentPtr>& inputArguments() const
        {
            return m_inputParams;
        }

        inline const std::map<std::string, OutputArgumentPtr>& outputArguments() const
        {
            return m_outputParams;
        }
        
        virtual std::shared_ptr<Algorithm> create() const = 0;

        static void Register(AlgorithmInfo * info);

        static const std::map<std::string, AlgorithmInfoPtr>& Get();


    protected:
        AlgorithmInfo(
            const std::string& name,
            std::initializer_list<std::pair<std::string, InputArgumentPtr>> in,
            std::initializer_list<std::pair<std::string, OutputArgumentPtr>> out);

    private:
        std::string                              m_name;
        std::map<std::string, InputArgumentPtr>  m_inputParams;
        std::map<std::string, OutputArgumentPtr> m_outputParams;

        static std::map<std::string, AlgorithmInfoPtr> m_algorithms;
    };
}

namespace Nan 
{
    namespace marshal
    {
        using namespace cloudcv;
        
        template<>
        struct Serializer<InputArgument>
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, InputArgument& val) = delete;

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const InputArgument& val)
            {
                val.serialize(ar);
            }
        };

        template<>
        struct Serializer<OutputArgument>
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, OutputArgument& val) = delete;

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const OutputArgument& val)
            {
                val.serialize(ar);
            }
        };

        template<>
        struct Serializer<AlgorithmInfo>
        {
            template<typename InputArchive>
            static inline void load(InputArchive& ar, AlgorithmInfo& val) = delete;

            template<typename OutputArchive>
            static inline void save(OutputArchive& ar, const AlgorithmInfo& val)
            {
                ar & make_nvp("name",            val.name());
                ar & make_nvp("inputArguments",  val.inputArguments());
                ar & make_nvp("outputArguments", val.outputArguments());
            }
        };
    }
}