/**
 * Defines image filter base classes.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 24. February 2019
 */
#pragma once

#include <memory>

#include "francor_vision/image.h"

namespace francor
{

namespace vision
{

class ImageFilterBase
{
protected:
  ImageFilterBase(const ColourSpace space) : required_colour_space_(space) { } 

public:
  ImageFilterBase(void) = delete;
  virtual ~ImageFilterBase(void) = default;

  ColourSpace requiredColourSpace(void) const { return required_colour_space_; }
  virtual bool isValid(void) const = 0;

private:
  ColourSpace required_colour_space_;
};

/**
 * Interface for image filter that creates a bitmask. The bitmask is also an input for each filter stage.
 */
class ImageFilter : public ImageFilterBase
{
protected:
  ImageFilter(const ColourSpace space) : ImageFilterBase(space) { }

public:
  ImageFilter(void) = delete;
  virtual ~ImageFilter(void) = default;

  bool process(Image& image) const
  {
    if (!this->safetyCheck(image))
      return false;

    return this->processImpl(image);    
  }

protected:
  virtual bool processImpl(Image& image) const = 0;

private:
  bool safetyCheck(const Image& image) const
  {
    return image.colourSpace() != ColourSpace::NONE;
  }
};

class ImageMaskFilter : public ImageFilterBase
{
protected:
  ImageMaskFilter(const ColourSpace space) : ImageFilterBase(space) { }

public:
  ImageMaskFilter(void) = delete;
  virtual ~ImageMaskFilter(void) = default;

  bool process(const Image& image, Image& mask) const
  {
    if (!this->safetyCheck(image, mask))
      return false;

    return this->processImpl(image, mask);
  }

protected:
  virtual bool processImpl(const Image& image, Image& mask) const = 0;

private:
  bool safetyCheck(const Image& image, Image& mask) const
  {
    return image.colourSpace() != ColourSpace::NONE
           &&
           mask.colourSpace() == ColourSpace::BIT_MASK
           &&
           image.cols() == mask.cols()
           &&
           image.rows() == mask.rows();
  }
};

} // end namespace vision

} // end namespace francor