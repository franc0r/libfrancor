/**
 * Defines the pixel class. A Pixel can contains more colour channels.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 22. February 2019
 */
#include <cassert>
#include <cstdint>
#include <stdexcept>

#include "francor_vision/colour_space.h"

namespace francor
{

namespace vision
{

/**
 * \brief Class Pixel that refers to the pixel data of an image.
 */
template <typename T>
class Pixel_
{
public:
  /**
   * \brief Creates a pixel class with a reference to a specific pixel of an image. The space is used for type safetiness.
   * 
   * \param data Pointer to the first byte of the pixel data.
   * \param space Colour space of the pixel.
   */
  Pixel_(T* const data, const ColourSpace space)
    : data_(data),
      colour_space_(space)
  {

  }

  /**
   * \brief Returns a reference to the red component of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the red component of the pixel.
   */
  inline T& r(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::BGR:
      return data_[2];
    case ColourSpace::RGB:
      return data_[0];
    default:
      throw std::out_of_range("Pixel::r(): red isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the green component of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the green component of the pixel.
   */
  inline T& g(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::BGR:
    case ColourSpace::RGB:
      return data_[1];
    default:
      throw std::out_of_range("Pixel::g(): green isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the blue component of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the blue component of the pixel.
   */
  inline T& b(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::BGR:
      return data_[0];
    case ColourSpace::RGB:
      return data_[2];
    default:
      throw std::out_of_range("Pixel::g(): green isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the hue component of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the hue component of the pixel.
   */
  inline T& h(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::HSV:
      return data_[0];
    default:
      throw std::out_of_range("Pixel::h(): hue isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the saturation component of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the saturation component of the pixel.
   */
  inline T& s(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::HSV:
      return data_[1];
    default:
      throw std::out_of_range("Pixel::s(): saturation isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the value (HSV) component of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the value (HSV) component of the pixel.
   */
  inline T& v(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::HSV:
      return data_[2];
    default:
      throw std::out_of_range("Pixel::v(): value isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the gray value of the pixel. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the gray value component of the pixel.
   */
  inline T& gray(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::GRAY:
      return data_[0];
    // TODO: implement a convertion from colour to gray.
    default:
      throw std::out_of_range("Pixel::gray(): gray isn't available.");
    }
  }

  /**
   * \brief Returns a reference to the value of the pixel of image type bit mask. If the pixel doesn't contain this colour a std::out_of_range exception is thrwon.
   * 
   * \return reference to the value of the pixel of image type bit mask.
   */
  inline T& bit(void)
  {
    switch (colour_space_)
    {
    case ColourSpace::BIT_MASK:
      return data_[0];
    default:
      throw std::out_of_range("Pixel::gray(): gray isn't available.");
    }
  }

private:
  T* const data_ = nullptr;
  const ColourSpace colour_space_ = ColourSpace::NONE;
};

using Pixel = Pixel_<std::uint8_t>;
using ConstPixel = Pixel_<const std::uint8_t>;

} // end namespace vision

} // end namespace francor