#include "francor_base/algorithm/impl/transform_eigen.h"
#include "francor_base/transform.h"

#include <Eigen/Dense>

#include <iostream>

namespace francor {

namespace base {

namespace algorithm {

namespace transform {

namespace eigen {

void transformPointVector(const Transform2d& transform, Point2dVector& points)
{
  using Map = Eigen::Map<Eigen::Matrix<double, 2, Eigen::Dynamic, Eigen::ColMajor>, Eigen::Aligned16, Eigen::InnerStride<2>>;
  Map point_mat(reinterpret_cast<double*>(points.data()), 2, points.size());
  std::cout << "point mat:" << std::endl << point_mat << std::endl;

  const Eigen::Translation2d t_t(transform.translation().x(), transform.translation().y());
  const Eigen::Rotation2Dd t_r(transform.rotation().phi());
  const Eigen::Vector2d t(t_t.translation());
  // Eigen::Transform<double, 2, Eigen::Affine> T =   *  ;
  // std::cout << "T:" << std::endl << T.matrix() << std::endl;
  point_mat = t_r.matrix() * point_mat;
  point_mat.colwise() + t;
  std::cout << "transformed point mat:" << std::endl << point_mat << std::endl;
}

} // end namespace eigen

} // end namespace transform

} // end namespace algorithm

} // end namespace base

} // end namespace francor