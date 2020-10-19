/**
 * This file defines a observation matrix that uses functions for state transformations (matrix multiplication) 
 * for state vectors and covariances.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 17. October 2020
 */
#pragma once

#include "francor_mapping/kinematic_state_vector.h"
#include <francor_base/matrix.h>

#include <tuple>

namespace francor {

namespace mapping {

namespace impl {

template <KinematicAttribute, KinematicAttribute, class>
struct transform_attribute;
template <KinematicAttribute InputAttribute, KinematicAttribute OutputAttribute, KinematicAttribute... InputAttributes>
struct transform_attribute<InputAttribute, OutputAttribute, mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>> {
  using output_type = typename resolve_type<OutputAttribute>::type;

  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>&)
  {
    return static_cast<output_type>(0);
  }
};

template <KinematicAttribute Attribute, KinematicAttribute... InputAttributes>
struct transform_attribute<Attribute, Attribute, mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>> {
  using output_type = typename resolve_type<Attribute>::type;

  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>& input)
  {
    return input.template value<Attribute>();
  }
};

template <KinematicAttribute... InputAttributes>
struct transform_attribute<KinematicAttribute::VEL, KinematicAttribute::VEL_X, mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>> {
  using output_type = typename resolve_type<KinematicAttribute::VEL_X>::type;

  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>& input)
  {
    return input.velocity() * std::cos(input.yaw());
  }
};

template <KinematicAttribute... InputAttributes>
struct transform_attribute<KinematicAttribute::VEL, KinematicAttribute::VEL_Y, mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>> {
  using output_type = typename resolve_type<KinematicAttribute::VEL_Y>::type;

  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>& input)
  {
    return input.velocity() * std::sin(input.yaw());
  }
};



template <typename, typename>
class ObservationMatrix2;

template <KinematicAttribute... ColAttributes, KinematicAttribute... RowAttributes>
class ObservationMatrix2<mapping::KinematicAttributePack<ColAttributes...>, mapping::KinematicAttributePack<RowAttributes...>>
{
private:
  // using Rows = std::tuple<ObservationMatrixRow<RowAttributes, ColAttributes...> ...>;
  
public:
  static constexpr std::size_t rows = sizeof...(RowAttributes);
  static constexpr std::size_t cols = sizeof...(ColAttributes);

  constexpr ObservationMatrix2() { }

  template <typename DataType, std::size_t InputMatrixCols>
  base::Matrix<DataType, rows, InputMatrixCols> transform(const base::Matrix<DataType, cols, InputMatrixCols>& matrix) const
  {
    base::Matrix<DataType, rows, InputMatrixCols> output;
    return { };
  }

  template <typename DataType>
  base::Matrix<DataType, cols, cols> transform(const base::Matrix<DataType, rows, rows>& matrix) const
  {
    return { };
  }

  mapping::KinematicStateVector<mapping::KinematicAttributePack<RowAttributes...>>
  operator*(const mapping::KinematicStateVector<mapping::KinematicAttributePack<ColAttributes...>>& input) const
  {
    mapping::KinematicStateVector<mapping::KinematicAttributePack<RowAttributes...>> output;

    ((output.template value<RowAttributes>() = static_cast<typename resolve_type<RowAttributes>::type>(0.0)), ...);
    (transformStateVector<ColAttributes>(input, output), ...);
  
    return output;
  }

private:
  using ColStateVector = mapping::KinematicStateVector<mapping::KinematicAttributePack<ColAttributes...>>;

  template <KinematicAttribute ColAttribute, KinematicAttribute... OutputAttributes>
  inline static void transformStateVector(const ColStateVector& input,
                                          mapping::KinematicStateVector<mapping::KinematicAttributePack<OutputAttributes...>>& output)
  {
    ((output.template value<OutputAttributes>() += transform_attribute<ColAttribute, OutputAttributes, ColStateVector>::f(input)), ...);
  }                                                                                    
};

} // end namespace impl

} // end namespace mapping

} // end namespace francor