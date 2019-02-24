/**
 * Expands the cv::Mat class for more type safetiness.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include "francor_vision/image.h"


namespace francor
{

namespace vision
{

Image::Image(const std::size_t rows, const std::size_t cols, const ColourSpace space)
  : colour_space_(space),
    data_source_(rows, cols, this->solveType(space)),
    data_(data_source_.data),
    stride_(data_source_.step),
    rows_(data_source_.rows),
    cols_(data_source_.cols)
{

}

Image::Image(const Image& image)
  : colour_space_(image.colour_space_),
    data_source_(image.data_source_.clone()),
    data_(data_source_.data),
    stride_(data_source_.step),
    rows_(data_source_.rows),
    cols_(data_source_.cols)
{

}

Image::Image(Image&& image)
  : colour_space_(image.colour_space_),
    data_source_(image.data_source_.clone()),
    data_(data_source_.data),
    stride_(data_source_.step),
    rows_(data_source_.rows),
    cols_(data_source_.cols)
{
  // clear all attributes and release memory of image
  image.clear();
}

Image::Image(const cv::Mat& mat, const ColourSpace space)
{
  this->copyFromCvMat(mat, space);
}

Image::Image(cv::Mat&& mat, const ColourSpace space)
{
  this->fromCvMat(mat, space);
  mat.release();
}

Image::~Image(void)
{

}

Image& Image::operator=(const Image& image)
{
  this->copyFromCvMat(image.data_source_, image.colour_space_);

  return *this;
}

Image& Image::operator=(Image&& image)
{
  this->fromCvMat(image.data_source_, image.colour_space_);
  image.clear();

  return *this;
}

void Image::clear(void)
{
  // release memory
  data_source_.release();
  data_ = nullptr;

  // reset image attributes
  colour_space_ = ColourSpace::NONE;
  stride_ = 0;
  rows_ = 0;
  cols_ = 0;
}

bool Image::fromCvMat(cv::Mat& mat, const ColourSpace space)
{
  // check if type matches the colour space
  if (Image::solveType(space) != mat.type())
  {
    // TODO throw exception
    return false;
  }

  // get reference to the data
  data_source_ = mat;
  data_ = data_source_.data;

  // get attributes from mat
  rows_ = data_source_.rows;
  cols_ = data_source_.cols;
  stride_ = data_source_.step;
  colour_space_ = space;

  // release memory reference from mat
  mat.release();

  return true;
}

bool Image::copyFromCvMat(const cv::Mat& mat, const ColourSpace space)
{
  // check if type matches the colour space
  if (Image::solveType(space) != mat.type())
  {
    // TODO throw exception
    return false;
  }

  // copy data from mat
  mat.copyTo(data_source_);
  data_ = data_source_.data;

  // get attributes from mat
  rows_ = data_source_.rows;
  cols_ = data_source_.cols;
  stride_ = data_source_.step;
  colour_space_ = space;

  return true;
}

void Image::resize(const std::size_t rows, const std::size_t cols, const ColourSpace space)
{
  const ColourSpace newSpace = (space == ColourSpace::NONE ? colour_space_ : space);
  
  data_source_.create(rows, cols, this->solveType(newSpace));
  rows_ = data_source_.rows;
  cols_ = data_source_.cols;
  stride_ = data_source_.step;
  colour_space_ = newSpace;
}

void Image::applyMask(const Image& mask)
{
  if (mask.colourSpace() != ColourSpace::BIT_MASK || rows_ != mask.rows_ || cols_ != mask.cols_)
    return;

  const std::size_t bytesPerPixel = this->solveBytesPerPixel(colour_space_);

  for (std::size_t row = 0; row < rows_; ++row)
    for (std::size_t col = 0; col < cols_; ++col)
      if (!mask(row, col).bit())
        for (std::size_t byte = 0; byte < bytesPerPixel; ++byte)
          data_[row * stride_ + col * byte] = 0;
}

int Image::solveType(const ColourSpace space)
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

std::size_t Image::solveBytesPerPixel(const ColourSpace space)
{
  switch (space)
  {
  case ColourSpace::BGR:
  case ColourSpace::HSV:
  case ColourSpace::RGB:
    return 3;

  case ColourSpace::BIT_MASK:
  case ColourSpace::GRAY:
    return 1;

  default:
    return 0;
  }
}

} // end namespace vision

} // end namespace francor