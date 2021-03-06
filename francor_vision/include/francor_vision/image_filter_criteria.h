/**
 * Defines and implement different image mask filters.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_vision/image_filter.h"

namespace francor {

namespace vision {

class ImageMaskFilterColourRange : public ImageMaskFilter
{
public:
  ImageMaskFilterColourRange(void) = delete;
  ImageMaskFilterColourRange(const std::uint8_t minH,
                             const std::uint8_t maxH,
                             const std::uint8_t minS,
                             const std::uint8_t maxS,
                             const std::uint8_t minV,
                             const std::uint8_t maxV)
  
    : ImageMaskFilter(ColourSpace::HSV),
      min_h_(minH),
      max_h_(maxH),
      min_s_(minS),
      max_s_(maxS),
      min_v_(minV),
      max_v_(maxV)
  {

  }

  virtual bool processImpl(const Image& image, Image& mask) const override final
  {
    // HSV colour space is required
    if (image.colourSpace() != ColourSpace::HSV)
    {
      // TODO: print error
      return false;
    }

    // work on opencv mat
    cv::Mat tempMask(mask.cvMat());
    cv::Mat resultMask;

    cv::inRange(image.cvMat(), cv::Scalar(min_h_, min_s_, min_v_), cv::Scalar(max_h_, max_s_, max_v_), resultMask);
    cv::bitwise_or(tempMask, resultMask, tempMask);

    // actually if tempMask wasn't reallocated there is no reason to use fromCvMat(), but we don't really know it
    mask.fromCvMat(tempMask, mask.colourSpace());

    return true;
  }

  virtual bool isValid(void) const override final
  {
    return min_h_ <= max_h_
           &&
           min_s_ <= max_s_
           &&
           min_v_ <= max_v_;
  }

private:
  const std::uint8_t min_h_;
  const std::uint8_t max_h_;
  const std::uint8_t min_s_;
  const std::uint8_t max_s_;
  const std::uint8_t min_v_;
  const std::uint8_t max_v_;
};

} // end namespace vision

} // end namespace francor