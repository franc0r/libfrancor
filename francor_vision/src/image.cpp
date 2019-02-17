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
  : cv::Mat(rows, cols, Image::solveType(space)),
    colour_space_(space)
{

}

Image::Image(const Image& image)
  : cv::Mat(image.clone()),
    colour_space_(image.colour_space_)
{

}

Image::Image(Image&& image)
  : cv::Mat(image),
    colour_space_(image.colour_space_)
{
  image.release();
}

Image::~Image(void)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

} // end namespace vision

} // end namespace francor