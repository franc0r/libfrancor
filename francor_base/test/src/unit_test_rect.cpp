#include <gtest/gtest.h>

#include <francor_base/rect.h>

using francor::base::Rect2u;
using francor::base::Rect2i;
using francor::base::Rect2f;
using francor::base::Rect2d;

TEST(Rect, DefaultConstruction)
{
  constexpr Rect2u rect2_u;
  constexpr Rect2i rect2_i;
  constexpr Rect2f rect2_f;
  constexpr Rect2d rect2_d;

  EXPECT_EQ(rect2_u.x(), 0);
  EXPECT_EQ(rect2_u.y(), 0);
  EXPECT_EQ(rect2_u.width(), 0);
  EXPECT_EQ(rect2_u.height(), 0);

  EXPECT_EQ(rect2_i.x(), 0);
  EXPECT_EQ(rect2_i.y(), 0);
  EXPECT_EQ(rect2_i.width(), 0);
  EXPECT_EQ(rect2_i.height(), 0);

  EXPECT_EQ(rect2_f.x(), 0.0f);
  EXPECT_EQ(rect2_f.y(), 0.0f);
  EXPECT_EQ(rect2_f.width(), 0.0f);
  EXPECT_EQ(rect2_f.height(), 0.0f);

  EXPECT_EQ(rect2_d.x(), 0.0);
  EXPECT_EQ(rect2_d.y(), 0.0);
  EXPECT_EQ(rect2_d.width(), 0.0);
  EXPECT_EQ(rect2_d.height(), 0.0);
}

TEST(Rect, TypeConvertion)
{
  constexpr Rect2u rect2_u({ 2, 6 }, { 1, 1 });
  constexpr Rect2i rect2_i({-2, 6 }, { 1, 1 });
  constexpr Rect2f rect2_f({2.5f, 6.5f }, { 1.0f, 1.0f });
  constexpr Rect2d rect2_d({2.5 , 6.5  }, { 1.0 , 1.0  });

  constexpr Rect2i to_int(rect2_d);
  constexpr Rect2f to_float(rect2_d);
  constexpr Rect2d to_double(rect2_u);

  EXPECT_EQ(to_int.x(), static_cast<int>(rect2_d.x()));
  EXPECT_EQ(to_int.y(), static_cast<int>(rect2_d.y()));

  EXPECT_EQ(to_float.x(), static_cast<float>(rect2_d.x()));
  EXPECT_EQ(to_float.y(), static_cast<float>(rect2_d.y()));

  EXPECT_EQ(to_double.x(), static_cast<double>(rect2_u.x()));
  EXPECT_EQ(to_double.y(), static_cast<double>(rect2_u.y()));
}