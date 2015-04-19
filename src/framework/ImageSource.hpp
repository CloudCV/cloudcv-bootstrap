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
#include <memory>
#include <nan.h>

namespace cloudcv
{
    /**
     * @brief   Image source is accessor to image, stored in external resource.
     * @details This abstract class has particular implementations to retrieve 
     *          image data from filesystem and image buffer.
     */
    class ImageSource
    {
    public:
        /**
         * @brief   Main method to retrieve image. This method will work synchronously.
         * @details Returns the image stored in the ImageSource object.
         * @return  This function returns the loaded image. It can also return an 
         *          empty object if the image could not been loaded.
         */
        virtual cv::Mat getImage(int flags = cv::IMREAD_COLOR) = 0;

        virtual ~ImageSource() {}

    protected:
        ImageSource() {}
    
    private:
        ImageSource(const ImageSource& src);
        ImageSource& operator=(const ImageSource& src);
    };

    typedef std::shared_ptr<ImageSource> ImageSourcePtr;

    /**
     * @brief Creates an ImageSource that points to particular file on filesystem.
     */
    ImageSourcePtr CreateImageSource(const std::string& filepath);

    /**
     * @brief Creates an ImageSource that points to file's binary content that was
     *        loaded using Node.js.
     */
    ImageSourcePtr CreateImageSource(v8::Local<v8::Object> imageBuffer);
}