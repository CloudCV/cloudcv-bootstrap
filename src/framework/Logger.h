#pragma once

#include <vector>
#include <array>
#include <iostream>
#include <iterator>
#include <opencv2/opencv.hpp>
#include <modules/analyze/dominantColors.hpp>

struct FunctionTraceLoggerCookie
{
    FunctionTraceLoggerCookie(const char * name) 
    : mName(name) 
    {
        std::clog << "Enter function: " << mName << std::endl;
    }

    ~FunctionTraceLoggerCookie()
    {
        std::clog << "Leave function: " << mName << std::endl;
    }

    const char * mName;
};


template <typename T>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    return out;
}

template <typename T, size_t Size>
inline std::ostream& operator<<(std::ostream& out, const std::array<T, Size>& v) {
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    return out;
}

/*
inline std::ostream& operator<<(std::ostream& out, const cv::Mat& res) {
    return out << "cv::Mat [ ... ]";
}
/*
template <typename T>
inline std::ostream& operator<<(std::ostream& out, const cv::Size_<T>& res) {
    return out << "cv::Size { width:" << res.width << ", height:" << res.height << "}";
}
*/
template <typename T>
inline std::ostream& operator<<(std::ostream& out, const cv::Rect_<T>& res) {
    return out << "cv::Rect { x:" << res.x << "; y:" << res.y << ", width:" << res.width << ", height:" << res.height << "}";
}

#if defined(_DEBUG) || defined(DEBUG) 
#if _MSC_VER
#define TRACE_FUNCTION FunctionTraceLoggerCookie(__FUNCTION__)
#else
#define TRACE_FUNCTION FunctionTraceLoggerCookie(__PRETTY_FUNCTION__)
#endif
#define LOG_TRACE_MESSAGE(x) std::cout << x << std::endl
#else
#define TRACE_FUNCTION          static_cast<void>(0)
#define LOG_TRACE_MESSAGE(x)    static_cast<void>(0)
#endif


