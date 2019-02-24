/**
 * Defines filter pipeline and strategy that are work on images.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include "francor_vision/image_filter.h"

#include <memory>

namespace francor
{

namespace vision
{

template <class T>
class ImageFilterPipeline_
{
public:
  ImageFilterPipeline_(void) = default;
  ImageFilterPipeline_(const ImageFilterPipeline_<T>&) = default;
  ImageFilterPipeline_(ImageFilterPipeline_<T>&&) = default;
  virtual ~ImageFilterPipeline_(void) = default;

  ImageFilterPipeline_<T>& operator=(const ImageFilterPipeline_<T>&) = default;
  ImageFilterPipeline_<T>& operator=(ImageFilterPipeline_<T>&&) = default;

  
  bool addFilter(const std::string& name, std::unique_ptr<const T> filter)
  {
    // check if pointer and filter is valid
    if (!filter || !filter->isValid())
    {
      // TODO: throw exception or print error
      return false;
    }
    // check if filter name is already used.
    if (filter_.find(name) != filter_.end())
    {
      // TODO: throw exception or print error
      return false;
    }

    // check if new required colour space must be added
    if (required_images_.find(filter->requiredColourSpace()) == required_images_.end())
    {
      required_images_[filter->requiredColourSpace()] = std::make_shared<Image>(0, 0, filter->requiredColourSpace());
    }

    // add filter
    filter_[name] = std::move(filter);

    return true;
  }

  bool isValid(void) const
  {
    bool valid = true;

    for (auto& filter : filter_)
      valid &= filter.second->isValid();

    return valid;
  }

protected:
  void createRequiredImages(const Image& image)
  {
    for (auto& it : required_images_)
    {
      // skip if image is from same type
      if (it.second->colourSpace() == image.colourSpace())
        continue;

      *it.second = std::move(Image(image, it.first));
    }
  }

  std::map<ColourSpace, std::shared_ptr<Image>> required_images_;
  std::map<std::string, std::unique_ptr<const T>> filter_;
};

class ImageFilterPipeline : public ImageFilterPipeline_<ImageFilter>
{
public:
  ImageFilterPipeline(void) = default;
  virtual ~ImageFilterPipeline(void) = default;

  bool process(Image& image)
  {
    for (auto& filter : filter_)
    {
      if (image.colourSpace() != filter.second->requiredColourSpace())
      {
        // TODO: print error
        return false;
      }
      
      if (!filter.second->process(image))
      {
        // TODO: print error
        return false;
      }
    }

    return true;
  }
};

class ImageMaskFilterPipeline : public ImageFilterPipeline_<ImageMaskFilter>
{
public:
  ImageMaskFilterPipeline(void) = default;
  virtual ~ImageMaskFilterPipeline(void) = default;

  bool process(const Image& image, Image& mask)
  {
    this->createRequiredImages(image);
    mask.resize(image.rows(), image.cols(), ColourSpace::BIT_MASK);

    for (auto& filter : filter_)
    {
      if (image.colourSpace() == filter.second->requiredColourSpace())
      {
        if (!filter.second->process(image, mask))
        {
          // TODO: print error
          return false;
        }
      }
      else
      {      
        if (!filter.second->process(*required_images_[filter.second->requiredColourSpace()], mask))
        {
          // TODO: print error
          return false;
        }
      }
    }

    return true;
  }
};

} // end namespace vision

} // end namespace francor