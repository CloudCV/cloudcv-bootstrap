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

#include <cloudcv.hpp>
#include <framework/marshal/marshal.hpp>

namespace cloudcv 
{
    NAN_METHOD(version)
    {
        NanScope();
        std::string versionString = lexical_cast(CV_MAJOR_VERSION) + "." + lexical_cast(CV_MINOR_VERSION);
        NanReturnValue(marshal(versionString));
    }

	NAN_METHOD(buildInformation)
    {
		NanEscapableScope();
        NanReturnValue(marshal(cv::getBuildInformation()));
    }
}