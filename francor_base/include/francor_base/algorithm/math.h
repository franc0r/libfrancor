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

template <typename Type = float>
constexpr auto pow(const Type base, const int exp)
{
  Type result = static_cast<decltype(result)>(1);

  if (exp >= 0) {
    for (int i = 0; i < exp; ++i) {
      result *= base;
    }
  }
  else {
    for (unsigned int i = 0; i < std::abs(exp); ++i) {
      result *= static_cast<decltype(result)>(1) / base;
    }
  }

  return result;
}



template <int Significant, int Exp, typename Type = float>
struct floating_number
{
  static constexpr Type value = static_cast<Type>(Significant) * pow<Type>(10, Exp);
};

template <typename Type>
constexpr auto fak(const Type n) -> decltype(n)
{
  Type result = 1;

  for (Type i = 2; i <= n; ++i) {
    result *= i;
  }

  return result;
}

template <unsigned int N, unsigned int K, typename Type = float>
struct binomial_coefficient
{
  static_assert(K <= N, "Support parameter K must be smaller equal N.");

  static constexpr Type value = fak<Type>(N) / (fak<Type>(K) * fak<Type>(N - K));
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

    static constexpr Type value = binomial_coefficient<N, K, Type>::value * pow<Type>(p, K) * pow<Type>(q, N - K);
  };

  // template <unsigned int A, unsigned int B>
  // struct cd
  // {
  //   static_assert(A <= B, "Template parameter A must be smaller equal B.");
  //   static_assert(B <= N, "Template parameter A and B must be smaller equal N.");

  //   static constexpr Type value = 
  // };
};

template <typename Type = float>
class BinomialCoefficient
{
public:
  constexpr BinomialCoefficient(const unsigned int n, const unsigned int k)
    : _value(fak<Type>(n) / (fak<Type>(k) * fak<Type>(n - k)))
  {
    assert(n >= k);
  }

  inline constexpr operator Type() const { return _value; }

private:
  const Type _value;
};

template <unsigned int N, typename Type = float>
class BinomialDistribution
{
public:
  BinomialDistribution() = delete;
  constexpr BinomialDistribution(const Type p)
    : _p(p),
      _q(static_cast<Type>(1) - p),
      _variance(static_cast<Type>(_n) * _p * _q),
      _mean(static_cast<Type>(_n) * _p)
  {
    assert(p >= 0.0 && p <= 1.0);

    for (unsigned int k = 0; k <= _n; ++k) {
      _pm[k] = BinomialCoefficient<Type>(_n, k) * pow<Type>(p, k) * pow<Type>(_q, _n - k);
    }
  }

  inline constexpr Type p() const { return _p; }
  inline constexpr Type q() const { return _q; }
  inline constexpr Type variance() const { return _variance; }
  inline constexpr Type mean() const { return _mean; }
  inline constexpr Type pm(const unsigned int k) const { return _pm[k]; }
  inline constexpr Type cd(const unsigned int k) const { assert(false); return _cd[k]; }
  inline constexpr unsigned int n() const { return _n; }

private:
  Type _p;
  Type _q;
  const unsigned int _n = N;
  Type _variance;
  Type _mean;
  Type _pm[N + 1] = { };
  Type _cd[N + 1] = { };
};

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor