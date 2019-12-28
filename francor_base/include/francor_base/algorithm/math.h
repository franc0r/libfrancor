/**
 * Math functions that aren't in std.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include <utility>

namespace francor {

namespace base {

namespace algorithm {

namespace math {

template <unsigned int EXP, typename Type = float>
struct pow
{
  pow() = delete;
  constexpr pow(const Type base) : value(calculate(base, std::make_integer_sequence<std::size_t, EXP>{})) { }
  const Type value;
  inline constexpr operator Type() const { return value; }

private:
  template <std::size_t... I>
  static constexpr Type calculate(const Type base, std::integer_sequence<std::size_t, I...>)
  {
    if constexpr (EXP == 0) {
      return static_cast<Type>(1);
    }
    else {
      return ((I * 0 + base) * ...);
    }
  }
};

template <typename Type, unsigned int... K>
inline constexpr Type fak(std::integer_sequence<unsigned int, K...>)
{
  if constexpr (sizeof...(K) == 0) {
    return static_cast<Type>(1);
  }
  else {
    return (static_cast<Type>(K + 1) * ...);
  }
}

template <std::size_t N, typename Type, typename Indices = std::make_integer_sequence<unsigned int, N>>
inline constexpr Type fak()
{
  return fak<Type>(Indices{});
}

template <typename Type>
inline constexpr Type fak(const Type value)
{
  Type result = 1;

  for (Type k = 1; k <= value; ++k) {
    result *= k;
  }

  return result;
}

template <unsigned int N, unsigned int K, typename Type = float>
struct binomial_coefficient
{
  static_assert(K <= N, "Support parameter K must be smaller equal N.");

  static constexpr Type value = fak<N, Type>() / (fak<K, Type>() * fak<N - K, Type>());
};

template <unsigned int N, unsigned int P, typename Type = float>
struct binomial_distribution
{
  static_assert(P <= N, "Template parameter P must be in range 0 .. N.");

  using type = Type;

  static constexpr Type p = static_cast<Type>(P) / static_cast<Type>(N);
  static constexpr Type q = static_cast<Type>(1) - p;
  static constexpr unsigned int n = N;
  static constexpr Type variance = static_cast<Type>(n) * p * q;
  static constexpr Type mean = n * p;
  
  template <unsigned int K>
  struct pmf
  {
    static_assert(K <= N, "Template parameter K must be smaller equal N.");

    static constexpr Type value = binomial_coefficient<N, K, Type>::value * pow<K, Type>(p) * pow<N - K, Type>(q);
  };
};

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor