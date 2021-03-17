#include "francor_mapping/algorithm/grid_impl.h"




namespace francor {

namespace mapping {

namespace algorithm {

namespace grid {

// explicit instantiation of template functions for all grid types
template void markLaserBeamBorder<OccupancyGrid>(OccupancyGrid&, const base::Point2d&, const base::AnglePiToPi, const base::Angle,
                                                 const double, const OccupancyGrid::cell_type&, const OccupancyGrid::cell_type&);
template void markLaserBeamBorder<TsdGrid>(TsdGrid&, const base::Point2d&, const base::AnglePiToPi, const base::Angle,
                                           const double, const TsdGrid::cell_type&, const TsdGrid::cell_type&);

template void fillMarkedShapes<OccupancyGrid>(OccupancyGrid&, const OccupancyGrid::cell_type&);
template void fillMarkedShapes<TsdGrid>(TsdGrid&, const TsdGrid::cell_type&);

template void registerLaserBeam<OccupancyGrid>(OccupancyGrid& grid,
                                               const base::Point2d&,
                                               const base::AnglePiToPi,
                                               const double,
                                               const OccupancyGrid::cell_type&,
                                               const OccupancyGrid::cell_type&); 
template void registerLaserBeam<TsdGrid>(TsdGrid& grid,
                                         const base::Point2d&,
                                         const base::AnglePiToPi,
                                         const double,
                                         const TsdGrid::cell_type&,
                                         const TsdGrid::cell_type&); 

template void registerLaserBeam<OccupancyGrid>(OccupancyGrid&,
                                               const base::Point2d&,
                                               const base::AnglePiToPi,
                                               const base::Angle,
                                               const double,
                                               const OccupancyGrid::cell_type&,
                                               const OccupancyGrid::cell_type&);
template void registerLaserBeam<TsdGrid>(TsdGrid&,
                                         const base::Point2d&,
                                         const base::AnglePiToPi,
                                         const base::Angle,
                                         const double,
                                         const TsdGrid::cell_type&,
                                         const TsdGrid::cell_type&);

template void registerLaserScan<OccupancyGrid>(OccupancyGrid&,
                                               const base::Pose2d&,
                                               const base::LaserScan&,
                                               const OccupancyGrid::cell_type&,
                                               const OccupancyGrid::cell_type&);
template void registerLaserScan<TsdGrid>(TsdGrid&,
                                         const base::Pose2d&,
                                         const base::LaserScan&,
                                         const TsdGrid::cell_type&,
                                         const TsdGrid::cell_type&);

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor