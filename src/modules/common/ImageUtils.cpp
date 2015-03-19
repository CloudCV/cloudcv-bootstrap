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

#include "ImageUtils.hpp"

namespace cloudcv
{
    void getOptimalSizeImage(cv::Mat input, cv::Mat& result)
    {
        const int maxWidth = 512;
        const int maxHeight = 512;

        if (input.rows > maxWidth || input.cols > maxHeight)
        {
            const int imgWidth = input.cols;
            const int imgHeight = input.rows;

            int width = input.cols;
            int height = input.rows;

            if (maxWidth && width > maxWidth)
            {
                width = maxWidth;
                height = (imgHeight * width / imgWidth);
            }

            if (maxHeight && height > maxHeight)
            {
                height = maxHeight;
                width = (imgWidth * height / imgHeight);
            }

            cv::resize(input, result, cv::Size(width, height));
        }
        else
        {
            input.copyTo(result);
        }
    }
}