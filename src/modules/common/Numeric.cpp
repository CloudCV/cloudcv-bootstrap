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

#include "Numeric.hpp"

namespace cloudcv
{

    size_t gcd(size_t u, size_t v)
    {
        // simple cases (termination)
        if (u == v)
            return u;

        if (u == 0)
            return v;

        if (v == 0)
            return u;

        // look for factors of 2
        if (~u & 1) // u is even
        {
            if (v & 1) // v is odd
                return gcd(u >> 1, v);
            else // both u and v are even
                return gcd(u >> 1, v >> 1) << 1;
        }

        if (~v & 1) // u is odd, v is even
            return gcd(u, v >> 1);

        // reduce larger argument
        if (u > v)
            return gcd((u - v) >> 1, v);

        return gcd((v - u) >> 1, u);
    }


    /**
     * Compute distribution of 1-D series
     */
    Distribution distribution(cv::InputArray data)
    {
        cv::Scalar avg, dev;
        double min, max;

        cv::meanStdDev(data, avg, dev);
        cv::minMaxLoc(data, &min, &max);

        Distribution d;
        d.average = avg.val[0];
        d.standardDeviation = dev.val[0];

        d.min = min;
        d.max = max;
        d.entropy = 0;

        return d;
    }

    std::ostream& operator<<(std::ostream& out, const Distribution& res)
    {
        return out << "Distribution {}";
    }
    std::ostream& operator<<(std::ostream& out, const RGBDistribution& res)
    {
        return out << "RGBDistribution { r:" << res.r << ", g:" << res.g << ", b:" << res.b << "}";
    }

}