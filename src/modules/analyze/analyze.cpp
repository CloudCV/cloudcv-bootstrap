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

#include "analyze.hpp"
#include "dominantColors.hpp"
#include "modules/common/ScopedTimer.hpp"
#include "modules/common/ImageUtils.hpp"

namespace cloudcv
{
    cv::Size ComputeImageAspectRatio(const cv::Size& frameSize)
    {
        int d = gcd(frameSize.width, frameSize.height);
        return cv::Size(frameSize.width / d, frameSize.height / d);
    }

    /**
    * Root mean square (RMS) contrast
    */
    double ComputeRmsContrast(cv::Mat_<unsigned char> grayscale)
    {
        cv::Mat I;
        grayscale.convertTo(I, CV_32F, 1.0f / 255.0f);

        cv::Mat normalized = (I - cv::mean(I).val[0]);

        double sum = cv::sum(normalized.mul(normalized)).val[0];
        double totalPixels = grayscale.rows * grayscale.cols;

        return sqrt(sum / totalPixels);
    }

    void AnalyzeImage(cv::Mat input, AnalyzeResult& value)
    {
        TRACE_FUNCTION;
        value = AnalyzeResult();

        DominantColorsExtractor colorsExtractor;
        colorsExtractor.process(input);

        value.frameSize = input.size();
        value.aspectRatio = ComputeImageAspectRatio(input.size());

        value.intensity = distribution(input);
        value.rmsContrast = ComputeRmsContrast(input);
        
        value.dominantColors = colorsExtractor.mainColors;

        value.uniqieColors = colorsExtractor.getUniqueColors();
        value.reducedColors = colorsExtractor.getRedicedColors();
        value.colorDeviation = colorsExtractor.getColorDeviation();
    }

}
