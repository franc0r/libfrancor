/**
 * This file defines a movement model for a ego vehicle using a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */

#pragma once

namespace francor {

namespace mapping {

class EgoModelKalman
{
public:
  EgoModelKalman();

  void reset();

private:
  
};

} // end namespace mapping

} // end namespace francor