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

#pragma once 

#include <opencv2/opencv.hpp>
#include <stdint.h>

struct ScopedTimer
{
    inline ScopedTimer()
    {
        m_startTime = cv::getTickCount();
    }
    
    inline double executionTimeMs() const
    {
        return (cv::getTickCount() - m_startTime) * 1000. / cv::getTickFrequency();
    }
    
    int64_t m_startTime;
};
