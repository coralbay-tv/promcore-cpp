#include "promcore/gauge.h"

#include <gtest/gtest.h>

namespace promcore {
namespace {

// Template test fixture
template <typename T>
class GaugeTest : public ::testing::Test {
protected:
  using GaugeType = T;
  using ValueType = decltype(std::declval<T>().Value());

  T gauge;
};

// Define the types to test
using GaugeTypes = ::testing::Types<GaugeDouble, GaugeInt>;
TYPED_TEST_SUITE(GaugeTest, GaugeTypes);

TYPED_TEST(GaugeTest, initialize_with_zero) {
  using ValueType = typename TestFixture::ValueType;
  EXPECT_EQ(this->gauge.Value(), ValueType{0});
}

TYPED_TEST(GaugeTest, inc) {
  this->gauge.Increment();

  using ValueType = typename TestFixture::ValueType;
  EXPECT_EQ(this->gauge.Value(), ValueType{1});
}

TYPED_TEST(GaugeTest, inc_number) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Increment(ValueType{4});
  EXPECT_EQ(this->gauge.Value(), ValueType{4});
}

TYPED_TEST(GaugeTest, inc_multiple) {
  this->gauge.Increment();
  this->gauge.Increment();
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Increment(ValueType{5});
  EXPECT_EQ(this->gauge.Value(), ValueType{7});
}

TYPED_TEST(GaugeTest, inc_negative_value) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Increment(ValueType{-1});
  EXPECT_EQ(this->gauge.Value(), ValueType{-1});
}

TYPED_TEST(GaugeTest, dec) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Set(ValueType{5});
  this->gauge.Decrement();
  EXPECT_EQ(this->gauge.Value(), ValueType{4});
}

TYPED_TEST(GaugeTest, dec_negative_value) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Decrement(ValueType{-1});
  EXPECT_EQ(this->gauge.Value(), ValueType{1});
}

TYPED_TEST(GaugeTest, dec_number) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Set(ValueType{5});
  this->gauge.Decrement(ValueType{3});
  EXPECT_EQ(this->gauge.Value(), ValueType{2});
}

TYPED_TEST(GaugeTest, set) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Set(ValueType{3});
  EXPECT_EQ(this->gauge.Value(), ValueType{3});
}

TYPED_TEST(GaugeTest, set_multiple) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.Set(ValueType{3});
  this->gauge.Set(ValueType{8});
  this->gauge.Set(ValueType{1});
  EXPECT_EQ(this->gauge.Value(), ValueType{1});
}

TYPED_TEST(GaugeTest, set_to_current_time) {
  using ValueType = typename TestFixture::ValueType;
  this->gauge.SetToCurrentTime();
  EXPECT_GT(this->gauge.Value(), ValueType{0});
}

}  // namespace
}  // namespace promcore
