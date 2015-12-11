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
#include <node.h>
#include <v8.h>
#include <nan.h>


namespace cloudcv
{
    /**
     * @brief   Image source is accessor to image, stored in external resource.
     * @details This abstract class has particular implementations to retrieve 
     *          image data from file system and image buffer.
     */
    class ImageView
    {
    public:
        /**
         * @brief   Main method to retrieve image. This method will work synchronously.
         * @details Returns the image stored in the ImageSource object.
         * @return  This function returns the loaded image. It can also return an 
         *          empty object if the image could not been loaded.
         */
        cv::Mat getImage(int flags) const;
        
        const cv::Mat& getImage() const;
        cv::Mat& getImage();

        virtual ~ImageView() {}

        /**
        * @brief Creates an ImageSource that points to particular file on filesystem.
        */
        static ImageView CreateImageSource(const std::string& filepath);

        /**
        * @brief Creates an ImageSource that points to file's binary content that was
        *        loaded using Node.js.
        */
        static ImageView CreateImageSource(v8::Local<v8::Value> bufferOrString);

        /**
        * @brief Creates an ImageSource that points to file's binary content that was
        *        loaded using Node.js.
        */
        static ImageView CreateImageSource(v8::Local<v8::Object> imageBuffer);

        class ImageSourceImpl;

        ImageView();

    protected:
        ImageView(std::shared_ptr<ImageSourceImpl> impl);


    private:
        std::shared_ptr<ImageSourceImpl> m_impl;
    };

    
}