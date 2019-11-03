/**
 * Provides helper functions to perform input and output data operations like save to file.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#pragma once

#include "francor_vision/image.h"

namespace francor
{

namespace vision
{

Image loadImageFromFile(const std::string& fileName, const ColourSpace space)
{
  cv::Mat mat;
  
  switch (space)
  {
  case ColourSpace::BGR:
    mat = cv::imread(fileName, cv::IMREAD_COLOR);
    break;

  case ColourSpace::GRAY:
    mat = cv::imread(fileName, cv::IMREAD_GRAYSCALE);
    break;

  default:
    //TODO: print error
    return { };
  }

  return { mat, space };
}

void saveImageToFile(const std::string& file_name, const Image& image)
{
  cv::imwrite(file_name, image.cvMat());
}

} // end namespace vision

} // end namespace francor