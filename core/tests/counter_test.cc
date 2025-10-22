#include "promcore/counter.h"

#include <gtest/gtest.h>

namespace promcore {
namespace {

// Template test fixture
template <typename T>
class CounterTest : public ::testing::Test {
protected:
  using CounterType = T;
  using ValueType = decltype(std::declval<T>().Value());

  T counter;
};

// Define the types to test
using CounterTypes = ::testing::Types<CounterDouble, CounterInt>;
TYPED_TEST_SUITE(CounterTest, CounterTypes);

TYPED_TEST(CounterTest, initialize_with_zero) {
  using ValueType = typename TestFixture::ValueType;
  EXPECT_EQ(this->counter.Value(), ValueType{0});
}

TYPED_TEST(CounterTest, inc) {
  this->counter.Increment();
  using ValueType = typename TestFixture::ValueType;
  EXPECT_EQ(this->counter.Value(), ValueType{1});
}

TYPED_TEST(CounterTest, inc_number) {
  using ValueType = typename TestFixture::ValueType;
  this->counter.Increment(ValueType{4});
  EXPECT_EQ(this->counter.Value(), ValueType{4});
}

TYPED_TEST(CounterTest, inc_multiple) {
  using ValueType = typename TestFixture::ValueType;
  this->counter.Increment();
  this->counter.Increment();
  this->counter.Increment(ValueType{5});
  EXPECT_EQ(this->counter.Value(), ValueType{7});
}

TYPED_TEST(CounterTest, inc_negative_value) {
  using ValueType = typename TestFixture::ValueType;
  this->counter.Increment(ValueType{5});
  this->counter.Increment(ValueType{-5});
  EXPECT_EQ(this->counter.Value(), ValueType{5});
}

TYPED_TEST(CounterTest, reset) {
  using ValueType = typename TestFixture::ValueType;
  this->counter.Increment();
  this->counter.Reset();
  EXPECT_EQ(this->counter.Value(), ValueType{0});
  this->counter.Increment(ValueType{5});
  this->counter.Increment();
  EXPECT_EQ(this->counter.Value(), ValueType{6});
}

}  // namespace
}  // namespace promcore
