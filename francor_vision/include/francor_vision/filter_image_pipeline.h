/**
 * Defines filter pipeline and strategy that are work on images.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include "francor_vision/image.h"

namespace francor
{

namespace vision
{

/**
 * Interface for image filter that creates a bitmask. The bitmask is also an input for each filter stage.
 */
class ImageMaskFilter
{
protected:
  ImageMaskFilter(void) = default;

public:
  virtual ~ImageMaskFilter(void) = default;

  virtual bool process(const cv::Mat& image, cv::Mat& mask) const = 0;

  static bool safetyCheck(const cv::Mat& image, cv::Mat& mask)
  {
    return image.type() == CV_8UC3
           &&
           mask.type() == CV_8UC1
           &&
           image.cols == mask.cols
           &&
           image.rows == mask.cols;
  }
};

using ImageMaskFilterPipeline = std::map<std::string, std::unique_ptr<ImageMaskFilter>>;

using ImageMaskPipelineStrategy = std::function<cv::Mat (const cv::Mat&, const ImageMaskFilterPipeline&)>;

} // end namespace vision

} // end namespace francor