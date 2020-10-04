/**
 * This file defines a builder class to create an observation matrix from StateAttributeVector.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include "francor_mapping/state_attributes.h"
#include <francor_base/matrix.h>

namespace francor {

namespace mapping {

// namespace impl {


template <typename, typename>
struct ObservationMatrix;

template <StateAttribute... AttributesA, StateAttribute... AttributesB>
struct ObservationMatrix<StateAttributeVector<AttributesA...>, StateAttributeVector<AttributesB...>>
{
  static constexpr std::size_t rows = sizeof...(AttributesA);
  static constexpr std::size_t cols = sizeof...(AttributesB);

  static inline constexpr double element(const std::size_t row, const std::size_t col) { return _values.data[row][col]; }

private:

  struct Array {
    constexpr Array() : data{0}
    {
      this->initializeData(std::make_index_sequence<rows>{}, std::make_index_sequence<cols>{});
    }

    template <std::size_t Row, std::size_t Col>
    static constexpr double estimateMatrixElementValue()
    {
      constexpr auto attribute_a = StateAttributeVector<AttributesA...>::template getAttributeByIndex<Row>();
      constexpr auto attribute_b = StateAttributeVector<AttributesB...>::template getAttributeByIndex<Col>();

      return attribute_a == attribute_b ? 1.0 : 0.0;
    }
  
    template <std::size_t Col, std::size_t... Rows>
    constexpr void initializeRow(std::index_sequence<Rows...>)
    {
      ( (data[Rows][Col] = estimateMatrixElementValue<Rows, Col>()), ... );
    }
      
    template <std::size_t... Rows, std::size_t... Cols>
    constexpr void initializeData(std::index_sequence<Rows...>, std::index_sequence<Cols...>)
    {
      ( initializeRow<Cols>(std::make_index_sequence<rows>{}), ... );
    }

    double data[rows][cols];
  };

  static constexpr Array _values{};
};

// } // end namespace impl

template <class VectorA, class VectorB>
struct ObservationMatrixBuilder;

template <StateAttribute... AttributesA, StateAttribute... AttributesB>
struct ObservationMatrixBuilder<StateAttributeVector<AttributesA...>, StateAttributeVector<AttributesB...>>
{
  using MatrixType = base::Matrix<double, sizeof...(AttributesA), sizeof...(AttributesB)>;

  static MatrixType build()
  {

  }

private:
  static constexpr double estimateMatrixEntry(const std::size_t row, const std::size_t col)
  {

  }
};


} // end namespace mapping

} // end namespace francor