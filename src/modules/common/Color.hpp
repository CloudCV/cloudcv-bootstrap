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

#include <stdint.h>

namespace cloudcv 
{

template <typename ValueType>
struct rgb_color_t
{
	typedef ValueType value_t;

	value_t r;	
	value_t g;	
	value_t b;	
};

typedef rgb_color_t<uint8_t> rgb8u_color_t;
typedef rgb_color_t<uint8_t> rgb32f_color_t;

}