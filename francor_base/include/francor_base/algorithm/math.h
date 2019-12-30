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



template <int EXP, typename Type = float>
struct pow
{
  pow() = delete;
  constexpr pow(const Type base) : value(calculate(base, std::make_integer_sequence<std::size_t, std::abs(EXP)>{})) { }
  inline constexpr operator Type() const { return value; }

  const Type value;
  using type = Type;

private:
  template <std::size_t... I>
  static constexpr Type calculate(const Type base, std::integer_sequence<std::size_t, I...>)
  {
    if constexpr (EXP == 0) {
      return static_cast<Type>(1);
    }
    else if constexpr (EXP < 0) {
      return ((I * 0 + 1 / base) * ...);
    }
    else {
      return ((I * 0 + base) * ...);
    }
  }
};

template <int Significant, int Exp, typename Type = float>
struct floating_number
{
  static constexpr Type value = static_cast<Type>(Significant) * pow<Exp, Type>(10);
};

template <unsigned int N, typename Type>
struct fak
{
  static constexpr Type value = fak<N, Type>::calculate(std::make_integer_sequence<unsigned int, N>{});

private:
  template <unsigned int... K>
  static constexpr Type calculate(std::integer_sequence<unsigned int, K...>)
  {
    if constexpr (sizeof...(K) == 0) {
      return static_cast<Type>(1);
    }
    else {
      return (static_cast<Type>(K + 1) * ...);
    }
  }
};

template <unsigned int N, unsigned int K, typename Type = float>
struct binomial_coefficient
{
  static_assert(K <= N, "Support parameter K must be smaller equal N.");

  static constexpr Type value = fak<N, Type>::value / (fak<K, Type>::value * fak<N - K, Type>::value);
};

template <unsigned int N, class P, typename Type = float>
struct binomial_distribution
{
  static_assert(P::value >= 0.0 && P::value <= 1.0, "Template parameter P must be in range 0 .. 1.");
  static_assert(std::is_floating_point<Type>::value, "Template argument Type should be an floating point.");

  using type = Type;

  static constexpr Type p = P::value;
  static constexpr Type q = static_cast<Type>(1) - p;
  static constexpr unsigned int n = N;
  static constexpr Type variance = static_cast<Type>(n) * p * q;
  static constexpr Type mean = n * p;
  
  template <unsigned int K>
  struct pm
  {
    static_assert(K <= N, "Template parameter K must be smaller equal N.");

    static constexpr Type value = binomial_coefficient<N, K, Type>::value * pow<K, Type>(p) * pow<N - K, Type>(q);
  };

  // template <unsigned int A, unsigned int B>
  // struct cd
  // {
  //   static_assert(A <= B, "Template parameter A must be smaller equal B.");
  //   static_assert(B <= N, "Template parameter A and B must be smaller equal N.");

  //   static constexpr Type value = 
  // };
};

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor