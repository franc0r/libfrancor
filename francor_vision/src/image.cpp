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
    data_storage_(rows * cols * Image::solveBytesPerPixel(space)),
    data_(data_storage_.data()),
    stride_(cols * Image::solveBytesPerPixel(space)),
    rows_(rows),
    cols_(cols)
{

}

Image::Image(const Image& image)
  : colour_space_(image.colour_space_),
    data_storage_(image.data_storage_),
    data_(data_storage_.data()),
    data_cv_mat_extern_(image.data_cv_mat_extern_),
    use_external_data_(image.use_external_data_),
    stride_(image.stride_),
    rows_(image.rows_),
    cols_(image.cols_)
{

}

Image::Image(Image&& image)
  : colour_space_(image.colour_space_),
    data_storage_(std::move(image.data_storage_)),
    data_(data_storage_.data()),
    data_cv_mat_extern_(image.data_cv_mat_extern_),
    use_external_data_(image.use_external_data_),
    stride_(image.stride_),
    rows_(image.rows_),
    cols_(image.cols_)
{
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
  // reset all attributes
  this->clear();

  if (image.use_external_data_)
  // use external cv::Mat data as source
  {
    data_cv_mat_extern_ = image.data_cv_mat_extern_;
    data_ = data_cv_mat_extern_.data;
    use_external_data_ = true;
  }
  else
  // copy image data
  {
    data_storage_ = image.data_storage_;
    data_ = data_storage_.data();  
  }

  // get image attributes
  colour_space_ = image.colour_space_;
  stride_ = image.stride_;
  rows_ = image.rows_;
  cols_ = image.cols_;

  return *this;
}

Image& Image::operator=(Image&& image)
{
  // reset all attributes
  this->clear();

  if (image.use_external_data_)
  // use external cv::Mat data as source
  {
    data_cv_mat_extern_ = image.data_cv_mat_extern_;
    data_ = data_cv_mat_extern_.data;
    use_external_data_ = true;
  }
  else
  // move image data
  {
    data_storage_ = std::move(image.data_storage_);
    data_ = data_storage_.data();  
  }

  // get image attributes
  colour_space_ = image.colour_space_;
  stride_ = image.stride_;
  rows_ = image.rows_;
  cols_ = image.cols_;

  // reset origin image
  image.clear();

  return *this;
}

void Image::clear(void)
{
  data_storage_.clear();
  data_cv_mat_extern_.release(); 
  data_ = nullptr;
  use_external_data_ = false;

  colour_space_ = ColourSpace::NONE;
  stride_ = 0;
  rows_ = 0;
  cols_ = 0;
}

bool Image::fromCvMat(const cv::Mat& mat, const ColourSpace space)
{
  // check if type matches the colour space
  if (Image::solveType(space) != mat.type())
  {
    // TODO throw exception
    return false;
  }

  // get attributes from mat
  rows_ = mat.rows;
  cols_ = mat.cols;
  stride_ = mat.step;
  colour_space_ = space;

  // TODO: not sure if the check below is valid at all
  //       - I think the reference counter of the cv::Mat should be checked, too. I read the counter is NULL if a external source is used.
  if (mat.data == data_storage_.data())
  // mat was exported before
  {
    data_ = data_storage_.data();
    use_external_data_ = false;
    data_cv_mat_extern_.release();
  }
  else
  {
    // register data usage
    data_cv_mat_extern_ = mat;
    data_ = data_cv_mat_extern_.data;
    data_storage_.clear();
    use_external_data_ = true;
  }

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

  // get attributes from mat
  this->clear();
  rows_ = mat.rows;
  cols_ = mat.cols;
  stride_ = mat.step;
  colour_space_ = space;

  // allocate memory and copy data from mat
  data_storage_.resize(mat.step * mat.rows);
  std::memcpy(data_storage_.data(), mat.data, data_storage_.size());
  data_ = data_storage_.data();

  return true;
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