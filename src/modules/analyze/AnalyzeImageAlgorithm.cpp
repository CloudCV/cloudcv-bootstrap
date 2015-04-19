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

#include "AnalyzeImageAlgorithm.hpp"
#include "framework/ScopedTimer.hpp"

#include <iomanip>
#include <set>

namespace cloudcv
{
    class DominantColorsExtractor
    {
    public:
        void process(const cv::Mat_<cv::Vec3b>& bgrImage);

        std::vector<DominantColor> mainColors;

        int getUniqueColors() const;
        int getRedicedColors() const;

    protected:
        typedef std::set<int> ColorsSet;

        bool findLargestColorSet(int similarityTolerance, int minPixelsInSet, const std::set<Color>& input, std::set<Color>& colorsSet) const;
        DominantColor computeFinalColor(const std::set<Color>& colorsSet) const;

    private:
        std::vector<unsigned char> rVec, gVec, bVec;
        std::map<int, int> fullColorsTable;
        std::map<int, int> quantizedColorsTable;

    };

    template<int N>
    static inline int quantize(int val)
    {
        val = val >> N;
        val = val << N;
        return val;
    }

    static inline int SQR(int val)
    {
        return val * val;
    }

    int Color::distanceTo(const cv::Vec3b& other) const
    {
        int r = hash & 0xFF;
        int g = (hash >> 8) & 0xFF;
        int b = (hash >> 16) & 0xFF;

        int other_r = other.val[2];
        int other_g = other.val[1];
        int other_b = other.val[0];

        return SQR(r - other_r) + SQR(g - other_g) + SQR(b - other_b);
    }

    int Color::distanceTo(const Color& other) const
    {
        int r = hash & 0xFF;
        int g = (hash >> 8) & 0xFF;
        int b = (hash >> 16) & 0xFF;

        int other_r = other.hash & 0xFF;
        int other_g = (other.hash >> 8) & 0xFF;
        int other_b = (other.hash >> 16) & 0xFF;

        return SQR(r - other_r) + SQR(g - other_g) + SQR(b - other_b);
    }

    std::string DominantColor::html() const
    {
        std::ostringstream sStream;
        sStream << "#" << std::hex << std::setw(2) << (int)color[2] // Red
            << std::hex << std::setw(2) << (int)color[1] // Green
            << std::hex << std::setw(2) << (int)color[0];// Blue
        return sStream.str();
    }

    void DominantColorsExtractor::process(const cv::Mat_<cv::Vec3b>& bgrImage)
    {
        rVec.clear();
        gVec.clear();
        bVec.clear();

        fullColorsTable.clear();
        quantizedColorsTable.clear();

        for (int row = 0; row < bgrImage.rows; row++)
        {
            for (int col = 0; col < bgrImage.cols; col++)
            {
                cv::Vec3b clr = bgrImage(row, col);

                int b = clr[0];
                int g = clr[1];
                int r = clr[2];

                int hash1 = r + (b << 8) + (g << 16);
                int hash2 = quantize<2>(r) +(quantize<2>(g) << 8) + (quantize<2>(b) << 16);

                fullColorsTable[hash1]++;
                quantizedColorsTable[hash2]++;

                rVec.push_back(r);
                gVec.push_back(g);
                bVec.push_back(b);
            }
        }

        std::set<Color> colors;

        for (std::map<int, int>::const_iterator it = quantizedColorsTable.begin(); it != quantizedColorsTable.end(); ++it)
        {
            colors.insert(Color(it->first, it->second));
        }

        mainColors.clear();

        std::set<Color> mfc;
        std::set<Color> colorsSet = colors;

        int minPixelsInSet = static_cast<int>(bgrImage.rows * bgrImage.cols * 0.02); // Minimum amount for dominant color is 5% of the area of the image

        while (findLargestColorSet(3000, minPixelsInSet, colorsSet, mfc))
        {
            std::set<Color> restColors;
            std::set_difference(colorsSet.begin(), colorsSet.end(), mfc.begin(), mfc.end(), std::inserter(restColors, restColors.end()));
            colorsSet = restColors;

            DominantColor c = computeFinalColor(mfc);
            mainColors.push_back(c);

        }
    }

    DominantColor DominantColorsExtractor::computeFinalColor(const std::set<Color>& colorsSet) const
    {
        float final_r = 0, final_g = 0, final_b = 0;

        int totalPixels = 0;

        typedef std::set<Color>::const_iterator It;

        for (It it = colorsSet.begin(); it != colorsSet.end(); it++)
        {
            totalPixels += it->count;
        }

        for (It it = colorsSet.begin(); it != colorsSet.end(); it++)
        {
            Color c = *it;

            int r = c.hash & 0xFF;
            int g = (c.hash >> 8) & 0xFF;
            int b = (c.hash >> 16) & 0xFF;

            float weight = (float)c.count / (float)totalPixels;

            final_r += weight * r;
            final_g += weight * g;
            final_b += weight * b;
        }

        DominantColor dc;

        dc.color = cv::Vec3b
            (
            cv::saturate_cast<uint8_t>(final_b),
            cv::saturate_cast<uint8_t>(final_g),
            cv::saturate_cast<uint8_t>(final_r)
            );

        dc.totalPixels = totalPixels;
        dc.error = 0;

        for (It it = colorsSet.begin(); it != colorsSet.end(); it++)
        {
            float weight = (float)it->count / (float)totalPixels;
            dc.error += weight * it->distanceTo(dc.color);
        }

        return dc;
    }

    bool DominantColorsExtractor::findLargestColorSet(int similarityTolerance, int minPixelsInSet, const std::set<Color>& input, std::set<Color>& colorsSet) const
    {
        colorsSet.clear();

        if (input.empty())
            return false;

        typedef std::set<Color>::const_iterator It;

        It largestColorIt = input.begin();
        int largestCount = largestColorIt->count;

        for (It it = input.begin(); it != input.end(); it++)
        {
            if (it->count > largestCount)
            {
                largestCount = it->count;
                largestColorIt = it;
            }
        }

        Color mfc = *largestColorIt;
        int totalPixelsInSet = 0;

        for (It it = input.begin(); it != input.end(); it++)
        {
            Color other = *it;
            int distance = mfc.distanceTo(other);

            if (distance < similarityTolerance)
            {
                totalPixelsInSet += other.count;
                colorsSet.insert(other);
            }
        }

        return !colorsSet.empty() && totalPixelsInSet > minPixelsInSet;
    }

    int DominantColorsExtractor::getUniqueColors() const
    {
        return (int)fullColorsTable.size();
    }

    int DominantColorsExtractor::getRedicedColors() const
    {
        return (int)quantizedColorsTable.size();
    }

    std::ostream& operator<<(std::ostream& out, const Color& res)
    {
        return out << "{ hash:" << res.hash << ", count: " << res.count << "}";
    }

    std::ostream& operator<<(std::ostream& out, const DominantColor& res)
    {
        return out << "{ color:" << res.color << ", totalPixels: " << res.totalPixels << "}";
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

        value.rmsContrast = ComputeRmsContrast(input);
        
        value.dominantColors = colorsExtractor.mainColors;

        value.uniqieColors = colorsExtractor.getUniqueColors();
        value.reducedColors = colorsExtractor.getRedicedColors();
    }

}
