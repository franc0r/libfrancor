/**
 * 
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include <random>

namespace francor {

namespace algorithm {

class Ransac
{
public:
  Ransac(void) = default;
  Ransac(const Ransac&) = default;
  Ransac(Ransac&&) = default;
  virtual ~Ransac(void) = default;

  Ransac& operator=(const Ransac&) = default;
  Ransac& operator=(Ransac&&) = default;

private:
  std::random_device rd_;
  std::mt19937 gen_;
};

} // end namespace algorithm

} // end namespace francor