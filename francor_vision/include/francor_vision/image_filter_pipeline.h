/**
 * Defines filter pipeline and strategy that are work on images.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <memory>

#include "francor_vision/image.h"

namespace francor
{

namespace vision
{

/**
 * Interface for image filter that creates a bitmask. The bitmask is also an input for each filter stage.
 */
class ImageFilter
{
protected:
  ImageFilter(void) = default;

public:
  virtual ~ImageFilter(void) = default;

  bool process(const Image& image) const
  {
    if (image.colourSpace() == ColourSpace::NONE)
      return false;

    return this->processImpl(image);    
  }

  bool process(const Image& image, Image& mask) const
  {
    if (this->safetyCheck(image, mask))
      return false;

    return this->processImpl(image, mask);
  }

  virtual ColourSpace requiredColourSpace(void) const = 0;
  virtual bool isValid(void) const = 0;

protected:
  virtual bool processImpl(const Image& image) const = 0;
  virtual bool processImpl(const Image& image, Image& mask) const = 0;

private:
  static bool safetyCheck(const Image& image, Image& mask)
  {
    return image.colourSpace() != ColourSpace::NONE
           &&
           image.cols() == mask.cols()
           &&
           image.rows() == mask.cols();
  }
};

class ImageFilterPipeline;
using ImageFilterPipelineStrategy = std::function<Image (const Image&, const ImageFilterPipeline&)>;

class ImageFilterPipeline
{
public:
  ImageFilterPipeline(void) = default;
  ImageFilterPipeline(const ImageFilterPipeline&) = default;
  ImageFilterPipeline(ImageFilterPipeline&&) = default;

  ImageFilterPipeline& operator=(const ImageFilterPipeline&) = default;
  ImageFilterPipeline& operator=(ImageFilterPipeline&&) = default;

  bool process(Image& image)
  {
    this->createRequiredImages(image);

    for (auto& filter : filter_)
    {
      if (image.colourSpace() == filter.second->requiredColourSpace())
        filter.second->process(image);
      else      
        filter.second->process(*required_images_[filter.second->requiredColourSpace()]);
    }
  }
  
  bool process(const Image& image, Image& mask)
  {
    this->createRequiredImages(image);

    for (auto& filter : filter_)
    {
      if (image.colourSpace() == filter.second->requiredColourSpace())
        filter.second->process(image, mask);
      else      
        filter.second->process(*required_images_[filter.second->requiredColourSpace()], mask);
    }
  }

  void addFilter(const std::string& name, std::unique_ptr<const ImageFilter> filter)
  {
    // check if pointer and filter is valid
    if (!filter || !filter->isValid())
    {
      // TODO: throw exception or print error
      return;
    }
    // check if filter name is already used.
    if (filter_.find(name) != filter_.end())
    {
      // TODO: throw exception or print error
      return;
    }

    // check if new required colour space must be added
    if (required_images_.find(filter->requiredColourSpace()) == required_images_.end())
    {
      required_images_[filter->requiredColourSpace()] = std::make_shared<Image>(0, 0, filter->requiredColourSpace());
    }

    // add filter
    filter_[name] = std::move(filter);
  }

  void setStrategy(ImageFilterPipelineStrategy strategy) { strategy_ = strategy; }

  bool isValid(void) const
  {
    bool valid = true;

    for (auto& filter : filter_)
      valid &= filter.second->isValid();

    valid &= strategy_.operator bool();

    return valid;
  }

private:
  void createRequiredImages(const Image& image)
  {
    for (auto& it : required_images_)
    {
      *it.second = std::move(Image(image, it.first));
    }
  }

  std::map<ColourSpace, std::shared_ptr<Image>> required_images_;
  std::map<std::string, std::unique_ptr<const ImageFilter>> filter_;
  ImageFilterPipelineStrategy strategy_;
};


} // end namespace vision

} // end namespace francor