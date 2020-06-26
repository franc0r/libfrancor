#include <gtest/gtest.h>

#include <francor_base/size.h>

using francor::base::Size2i;
using francor::base::Size2d;
using francor::base::Size2u;

TEST(Size, DefaultConstructed)
{
  constexpr Size2i size_i();
  constexpr Size2u size_u();
  constexpr Size2d size_d();
}

TEST(Size, TypeConvertion)
{
  constexpr Size2i size_i(-1, 1);
  constexpr Size2u size_u(2, 3);
  constexpr Size2d size_d(2.5, 3.5);

  constexpr Size2d toDouble(size_i);
  constexpr Size2i toInt(size_d);
  constexpr Size2u toUnsigned(size_i);
}