/**
 * Expands the cv::Mat class for more type safetiness.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_vision/pixel.h"

#include <vector>

#include <opencv2/opencv.hpp>

namespace francor
{

namespace vision
{

class Image
{
public:

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
   * \brief Constructs a image from a cv::Mat. The colour space must be given. The cv::Mat image data will be copied.
   * \param mat Original image. The data are copied from the original image.
   * \param colourSpace The colour space of the image. It must be chosen manually.
   */
  Image(const cv::Mat& mat, const ColourSpace space);

  /**
   * \brief Constructs a image that refers to the cv::Mat data. The colour space must be given. No data will be coped.
   * \param mat Original image. No data will be copied from the original image.
   * \param colourSpace The colour space of the image. It must be chosen manually.
   */
  Image(cv::Mat&& mat, const ColourSpace space);

  /**
   * \brief Constructs a copy of the given image. Corrects the copy contructor of cv::Mat, because it is actually 
   *        like a shared pointer.
   * \param image Origin image that will be copied.
   */
  Image(const Image& image);

  Image(const Image& image, const ColourSpace space)
  {
    *this = image;
    this->transformTo(space);
  }

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
   * \brief Returns the pixel at (row, col).
   * 
   * \param row
   * \param col
   * \return pixel that contains references to the data
   */
  inline Pixel operator()(const std::size_t row, const std::size_t col) { return { data_ + (row * stride_ + col * Image::solveBytesPerPixel(colour_space_)), colour_space_ }; }

  /**
   * \brief Returns the const pixel at (row, col).
   * 
   * \param row
   * \param col
   * \return pixel that contains const references to the data
   */
  inline ConstPixel operator()(const std::size_t row, const std::size_t col) const { return { data_ + (row * stride_ + col * Image::solveBytesPerPixel(colour_space_)), colour_space_ }; }
  
  /**
   * \brief Return the number of rows of this image.
   * 
   * \return number of rows of this image
   */
  inline std::size_t rows(void) const noexcept { return rows_; }

  /**
   * \brief Return the number of columns of this image.
   * 
   * \return number of columns of this image
   */  
  inline std::size_t cols(void) const noexcept { return cols_; }

  /**
   * \brief Creates a cv::Mat that uses the data of this image. That can be used for opencv image operations. No data are copied.
   * 
   * \return cv::Mat that uses data of this image without coping of data
   */
  inline cv::Mat cvMat(void) const { return data_source_; }

  /**
   * \brief Gets a refernce to the data of a cv::Mat. The cv::Mat uses internally shared memory. The reference counter of the data will be incremented. No data are copied.
   * 
   * \param mat It will be established a reference to the image data of mat. mat will be released.
   * \param space The colour space of the image. The space must be match to the opencv type (CV_8UC1: Gray, BitMask; CV_8UC3: RGB, BGR; HSV).
   * \return true if no error is orrucred.
   */
  bool fromCvMat(cv::Mat& mat, const ColourSpace space);

  /**
   * \brief Copys the data from a cv::Mat.
   * 
   * \param mat The data of mat will be copied.
   * \param space The colour space of the image. The space must be match to the opencv type (CV_8UC1: Gray, BitMask; CV_8UC3: RGB, BGR; HSV).
   * \return true if no error is orrucred.
   */
  bool copyFromCvMat(const cv::Mat& mat, const ColourSpace space);

  /**
   * \brief Resets this image. All attributes will be set to default values. If a cv::Mat is used as external data source the reference will be removed.
   */
  void clear(void);

  /**
   * \brief Resize this image using the given colour space.
   * 
   * \param rows New number of rows.
   * \param cols New number of columns.
   * \param space New colour space.
   */
  void resize(const std::size_t rows, const std::size_t cols, const ColourSpace space = ColourSpace::NONE);

  /**
   * \brief Mask this image using a bit mask. If the mask bit is zero the correspond pixel will be set to zero. The mask must be of the same size.
   * 
   * \param mask Bit mask that is used to mask this image. mask must be of the same size.
   */
  void applyMask(const Image& mask);

  // TODO: complete the implementation 
  bool transformTo(const ColourSpace space)
  {
    cv::Mat mat(this->cvMat());

    switch (colour_space_)
    {
    case ColourSpace::BGR:
      {
        switch (space)
        {
        case ColourSpace::GRAY:
          cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
          break;

        case ColourSpace::HSV:
          cv::cvtColor(mat, mat, cv::COLOR_BGR2HSV);
          break;

        case ColourSpace::RGB:
          cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
          break;

        default:
          return false;
        }
      }

      break;

    case ColourSpace::RGB:
      {
        switch (space)
        {
        case ColourSpace::GRAY:
          cv::cvtColor(mat, mat, cv::COLOR_RGB2GRAY);
          break;

        case ColourSpace::HSV:
          cv::cvtColor(mat, mat, cv::COLOR_RGB2HSV);
          break;

        case ColourSpace::BGR:
          cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
          break;

        default:
          return false;
        }
      }

      break;

    case ColourSpace::GRAY:
      {
        switch (space)
        {
        case ColourSpace::RGB:
          cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);
          break;
        
        case ColourSpace::BGR:
          cv::cvtColor(mat, mat, cv::COLOR_GRAY2BGR);
          break;

        default:
          break;
        }
      }

      break;

    // TODO: add implementation for HSV and GRAY, too 

    default:
      return false;
    }

    // read back the image attributes
    this->fromCvMat(mat, space);

    return true;
  }

  /**
   * \brief Creates a image with the given size and colour space and initialize it with zeros.
   * 
   * \param rows The number of rows of the image.
   * \param cols The number of cols of the image.
   * \param space The colour space of the image.
   * \return New allocated image if no error was occurred otherwise an empty image.
   */
  static Image zeros(const std::size_t rows, const std::size_t cols, const ColourSpace space);

private:

  /**
   * \brief Solves the colour space to the cooresponding open cv mat type.
   * \param space Colour space.
   * \return open cv mat type.
   */
  static int solveType(const ColourSpace space);

  static std::size_t solveBytesPerPixel(const ColourSpace space);

  // members
  ColourSpace colour_space_ = ColourSpace::NONE;

  cv::Mat data_source_; // uses cv::Mat to allocate and handle the image data
  std::uint8_t* data_ = nullptr;

  std::size_t stride_ = 0;
  std::size_t rows_ = 0;
  std::size_t cols_ = 0;
};

} // end namespace vision

} // end namespace francor