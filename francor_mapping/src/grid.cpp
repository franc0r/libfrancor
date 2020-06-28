#include "grid_impl.cpp"

#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/tsd_grid.h"


namespace francor {

namespace mapping {

// explicit instantiation of GridDataMemoryHandler class
template class GridDataMemoryHandler<OccupancyCell>;
template class GridDataMemoryHandler<TsdCell>;
template class GridDataMemoryHandler<double>;
template class GridDataMemoryHandler<float>;
template class GridDataMemoryHandler<int>;

// explicit instantiation of Grid class
template class Grid<OccupancyCell>;
template class Grid<TsdCell>;
template class Grid<double>;
template class Grid<float>;
template class Grid<int>;

} // end namespace mapping

} // end namespace francor