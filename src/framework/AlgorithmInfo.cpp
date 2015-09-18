#include "framework/AlgorithmInfo.hpp"
#include "framework/AlgorithmExceptions.hpp"

namespace cloudcv
{
    AlgorithmInfo::AlgorithmInfo(
        const std::string& name,
        std::initializer_list<std::pair<std::string, InputArgumentPtr>> in,
        std::initializer_list<std::pair<std::string, OutputArgumentPtr>> out)
        : m_name(name)

    {
        for (auto i : in)
        {
            auto res = m_inputParams.insert(i);
            if (!res.second)
                throw ArgumentException(i.first, "Duplicate argument name");
        }

        for (auto o : out)
        {
            auto res = m_outputParams.insert(o);
            if (!res.second)
                throw ArgumentException(o.first, "Duplicate argument name");
        }

    }
}