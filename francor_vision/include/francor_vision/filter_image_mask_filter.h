/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include "francor_vision/filter_image_pipeline.h"

namespace francor {

namespace base {

class ImageMaskFilterColour : public ImageMaskFilter
{
public:
  ImageMaskFilterColour(void) = delete;
  ImageMaskFilterColour(const std::uint8_t minH,
                        const std::uint8_t maxH,
                        const std::uint8_t minS,
                        const std::uint8_t maxS,
                        const std::uint8_t minV,
                        const std::uint8_t maxV)
    : min_h_(minH),
      max_h_(maxH),
      min_s_(minS),
      max_s_(maxS),
      min_v_(minV),
      max_v_(maxV)
  {

  }

  virtual bool process(const cv::Mat& image, cv::Mat& mask) const override
  {
    if (!this->safetyCheck(image, mask))
      return false;

    cv::Mat tempMask;

    cv::inRange(image, cv::Scalar(min_h_, min_s_, min_v_), cv::Scalar(max_h_, max_s_, max_v_), tempMask);
    cv::bitwise_or(mask, tempMask, mask);

    return true;
  }

private:
  const std::uint8_t min_h_;
  const std::uint8_t max_h_;
  const std::uint8_t min_s_;
  const std::uint8_t max_s_;
  const std::uint8_t min_v_;
  const std::uint8_t max_v_;
};

} // end namespace perception

} // end namespace francor