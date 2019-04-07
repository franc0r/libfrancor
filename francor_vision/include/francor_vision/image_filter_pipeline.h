/**
 * Defines filter pipeline and strategy that are work on images.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

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
    auto compare = [=] (const std::pair<const std::string, std::unique_ptr<const T>>& element) { return element.first == name; };

    if (std::find_if(filter_.begin(), filter_.end(), compare) != filter_.end())
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
    filter_.push_back(std::make_pair(name, std::move(filter)));

    return true;
  }

  bool isValid(void) const
  {
    bool valid = true;

    for (auto& filter : filter_)
      valid &= filter.second->isValid();

    return valid;
  }

  std::size_t numOfFilters(void) const noexcept { return filter_.size(); }

protected:
  std::map<ColourSpace, std::shared_ptr<Image>> createRequiredImages(const Image& image) const
  {
    std::map<ColourSpace, std::shared_ptr<Image>> images(required_images_);

    for (auto& it : images)
    {
      // skip if image is from same type
      if (it.first == image.colourSpace())
        continue;

      it.second = std::make_shared<Image>(image, it.first);
    }

    return std::move(images);
  }

  std::map<ColourSpace, std::shared_ptr<Image>> required_images_;
  std::vector<std::pair<const std::string, std::unique_ptr<const T>>> filter_;
};

class ImageFilterPipeline : public ImageFilterPipeline_<ImageFilter>
{
public:
  ImageFilterPipeline(void) = default;
  virtual ~ImageFilterPipeline(void) = default;

  bool operator()(const Image& input, Image& output) const
  {
    const Image* image = &input;

    for (auto& filter : filter_)
    {
      if (image->colourSpace() != filter.second->requiredColourSpace())
      {
        // TODO: print error
        return false;
      }
      
      if (!filter.second->process(*image, output))
      {
        // TODO: print error
        return false;
      }

      image = &output;
    }

    return true;
  }
};

class ImageMaskFilterPipeline : public ImageFilterPipeline_<ImageMaskFilter>
{
public:
  ImageMaskFilterPipeline(void) = default;
  virtual ~ImageMaskFilterPipeline(void) = default;

  bool operator()(const Image& image, Image& mask) const
  {
    // prepare mask for filter operations even there is no filter added
    mask = std::move(Image::zeros(image.rows(), image.cols(), ColourSpace::BIT_MASK));

    // return if there is no filter added otherwise the required images access is not initialized properly
    if (filter_.size() == 0)
      return true;

    auto requiredImages(this->createRequiredImages(image));

    for (auto& filter : filter_)
    {
      // use image as input if the colour spaces matche
      if (image.colourSpace() == filter.second->requiredColourSpace())
      {
        if (!filter.second->process(image, mask))
        {
          // TODO: print error
          return false;
        }
      }
      // use image with correct colour space if origin image's space doesn't match
      else
      {
        if (!filter.second->process(*requiredImages[filter.second->requiredColourSpace()], mask))
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