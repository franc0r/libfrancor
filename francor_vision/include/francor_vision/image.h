/**
 * Expands the cv::Mat class for more type safetiness.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <opencv2/opencv.hpp>

namespace francor
{

namespace vision
{

class Image : public cv::Mat
{
public:

  enum class ColourSpace {
    NONE,
    GRAY,
    BGR,
    RGB,
    HSV,
    BIT_MASK,
  };

  /**
   * \brief Default constructor. A empty image will be constructed.
   */
  Image(void) = default;

  /**
   * \brief Constructs a image with the given rows, cols and colour space.
   * \param rows Number of rows of the construced image.
   * \param cols Number of cols of the construced image.
   * \param space The colour space of the construced image.
   */
  Image(const std::size_t rows, const std::size_t cols, const ColourSpace space);

  /**
   * \brief Constructs a copy of the given image. Corrects the copy contructor of cv::Mat, because it is actually 
   *        like a shared pointer.
   * \param image Origin image that will be copied.
   */
  Image(const Image& image);

  /**
   * \brief Move constructor. Corrects the copy contructor of cv::Mat, because it is actually like a shared pointer.
   *        This constructor releases the data from the origin image.
   * 
   * \param image origin image. After the origin is moved the contained data are released.
   */
  Image(Image&& image);

  /**
   * \brief Destructor.
   */
  ~Image(void);

  /**
   * \brief Copy assignment operator. Creates a copy of the origin image.
   * 
   * \param image Origin image that will be copied.
   * \return a reference to this image.
   */
  Image& operator=(const Image& image);

  /**
   * \brief Move assignment operator. Corrects the copy assignment operator of cv::Mat, because it is actually like a shared
   *        pointer. This constructor releases the data from the origin image.
   * 
   * \param image Origin image that will be moved.
   * \return a reference to this image.
   */
  Image& operator=(Image&& image);

  /**
   * Returns the current colour space of this image.
   * 
   * \return colour space of this image.
   */
  ColourSpace colourSpace(void) const noexcept { return colour_space_; }

  /**
   * \brief Solves the colour space to the cooresponding open cv mat type.
   * \param space Colour space.
   * \return open cv mat type.
   */
  static int solveType(const ColourSpace space)
  {
    switch (space)
    {
      case ColourSpace::BGR:
      case ColourSpace::RGB:
      case ColourSpace::HSV:
        return CV_8UC3;

      case ColourSpace::GRAY:
      case ColourSpace::BIT_MASK:
        return CV_8UC1;

      default:
        return CV_8UC1;
    }
  }

private:
  ColourSpace colour_space_ = ColourSpace::NONE;
};

} // end namespace vision

} // end namespace francor