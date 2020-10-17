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

template <KinematicAttribute InputAttribute, KinematicAttribute OutputAttribute>
struct transform_attribute {
  using input_type  = typename resolve_type<InputAttribute>::type;
  using output_type = typename resolve_type<OutputAttribute>::type;

  template <KinematicAttribute... InputAttributes>
  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>&)
  {
    return static_cast<output_type>(0);
  }
};

template <KinematicAttribute Attribute>
struct transform_attribute<Attribute, Attribute> {
  using input_type  = typename resolve_type<Attribute>::type;
  using output_type = typename resolve_type<Attribute>::type;

  template <KinematicAttribute... InputAttributes>
  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>& input)
  {
    return input.template value<Attribute>();
  }
};

template <>
struct transform_attribute<KinematicAttribute::VEL, KinematicAttribute::VEL_X> {
  using input_type  = typename resolve_type<KinematicAttribute::VEL>::type;
  using output_type = typename resolve_type<KinematicAttribute::VEL_X>::type;

  template <KinematicAttribute... InputAttributes>
  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>& input)
  {
    static_assert(input.template hasAttribute<KinematicAttribute::YAW>(), "Transformation from VEL to VEL_X requires attribute yaw in input state vector.");
    return input.template value<input_type>() * std::cos(input.template value<KinematicAttribute::YAW>());
  }
};

template <>
struct transform_attribute<KinematicAttribute::VEL, KinematicAttribute::VEL_Y> {
  using input_type  = typename resolve_type<KinematicAttribute::VEL>::type;
  using output_type = typename resolve_type<KinematicAttribute::VEL_Y>::type;

  template <KinematicAttribute... InputAttributes>
  inline static constexpr output_type f(const mapping::KinematicStateVector<mapping::KinematicAttributePack<InputAttributes...>>& input)
  {
    static_assert(input.template hasAttribute<KinematicAttribute::YAW>(), "Transformation from VEL to VEL_X requires attribute yaw in input state vector.");
    return input.template value<input_type>() * std::sin(input.template value<KinematicAttribute::YAW>());
  }
};

// template <KinematicAttribute RowAttribute, KinematicAttribute... ColAttributes>
// class ObservationMatrixRow
// {
// private:
//   using RowTransformOperators = std::tuple<transform_attribute<RowAttribute, ColAttributes>...>;

// public:
//   template <std::size_t Col>
//   using TransformOperator = typename std::tuple_element_t<Col, RowTransformOperators>::f;
// };



// template <std::size_t Row,
//           std::size_t Col,
//           KinematicAttribute RowAttribute,
//           KinematicAttribute HeadColAttribute,
//           KinematicAttribute... TailColAttributes>
// class ObservationMatrixRowImpl<Row, Col, RowAttribute, HeadColAttribute, TailColAttributes...>
//   : public ObservationMatrixRowImpl<Row, Col, RowAttribute, HeadColAttribute, TailColAttributes...>

template <typename, typename>
class ObservationMatrix2;

template <KinematicAttribute... RowAttributes, KinematicAttribute... ColAttributes>
class ObservationMatrix2<mapping::KinematicAttributePack<RowAttributes...>, mapping::KinematicAttributePack<ColAttributes...>>
{
private:
  // using Rows = std::tuple<ObservationMatrixRow<RowAttributes, ColAttributes...> ...>;
  
public:
  static constexpr std::size_t rows = sizeof...(RowAttributes);
  static constexpr std::size_t cols = sizeof...(ColAttributes);

  constexpr ObservationMatrix2() { }

  template <typename DataType>
  base::Matrix<DataType, cols, cols> operator*(const base::Matrix<DataType, rows, rows>& matrix) const
  {
    base::Matrix<DataType, cols, cols> result;

    // transform<DataType, MatrixCols>(matrix, result, std::make_index_sequence<cols>{});
    return result;
  }

  mapping::KinematicStateVector<mapping::KinematicAttributePack<ColAttributes...>>
  operator*(const mapping::KinematicStateVector<mapping::KinematicAttributePack<RowAttributes...>>& input) const
  {
    mapping::KinematicStateVector<mapping::KinematicAttributePack<ColAttributes...>> output;

    ((output.template value<ColAttributes>() = static_cast<typename resolve_type<ColAttributes>::type>(0.0)), ...);
    ((transformAttributeValue<RowAttributes>(input, output)), ...);
  
    return output;
  }

private:
  template <KinematicAttribute InputAttribute, KinematicAttribute... OutputAttributes>
  inline static void transformAttributeValue(const mapping::KinematicStateVector<mapping::KinematicAttributePack<RowAttributes...>>& input,
                                             mapping::KinematicStateVector<mapping::KinematicAttributePack<OutputAttributes...>>& output)
  {
    ((output.template value<OutputAttributes>() += transform_attribute<InputAttribute, OutputAttributes>::f(input)), ...);
  }                                                                                    

  // template <std::size_t Row, std::size_t Col, typename DataType, std::size_t MatrixCols, std::size_t... Indices>
  // inline static DataType transformElement(const base::Matrix<DataType, cols, MatrixCols>& input, std::index_sequence<Indices...>)
  // {
  //   DataType result(static_cast<DataType>(0));

  //   ((result += f<Row, Indices>(input(Indices, Col))), ...);
    
  //   return result;
  // }

  // template <std::size_t Row, typename DataType, std::size_t MatrixCols, std::size_t... ColIndices>
  // inline static void transformRow(const base::Matrix<DataType, cols, MatrixCols>& input,
  //                                 base::Matrix<DataType, rows, MatrixCols>& result,
  //                                 std::index_sequence<ColIndices...>)
  // {
  //   ((result(Row, ColIndices) = transformElement<Row, ColIndices, DataType, MatrixCols>(input, std::make_index_sequence<cols>{})), ...);
  // }

  // template <typename DataType, std::size_t... RowIndices>
  // inline static void transform(const base::Matrix<DataType, rows, rows>& input,
  //                              base::Matrix<DataType, cols, cols>& result,
  //                              std::index_sequence<RowIndices...>)
  // {
  //   (transformRow<RowIndices, DataType, cols>(input, std::make_index_sequence<cols>{}), ...);
  // }


  // template <std::size_t Row, std::size_t Col>
  // using f = typename std::tuple_element_t<Row, Rows>::template TransformOperator<Col>;
};

} // end namespace impl

} // end namespace mapping

} // end namespace francor