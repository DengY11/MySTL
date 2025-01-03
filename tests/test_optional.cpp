#include "optional.hpp"
#include <gtest/gtest.h>
#include <string>
using namespace MySTL;

// 测试默认构造函数
TEST(OptionalTest, DefaultConstructor) {
  optional<int> opt;
  EXPECT_FALSE(opt.has_value());
}

// 测试值构造函数
TEST(OptionalTest, ValueConstructor) {
  optional<int> opt(42);
  EXPECT_TRUE(opt.has_value());
  EXPECT_EQ(opt.value(), 42);
}

// 测试nullopt构造函数
TEST(OptionalTest, NulloptConstructor) {
  optional<int> opt(nullopt);
  EXPECT_FALSE(opt.has_value());
}

// 测试in_place构造函数
TEST(OptionalTest, InPlaceConstructor) {
  struct Test {
    int x, y;
  };
  optional<Test> opt(in_place, 1, 2);
  EXPECT_TRUE(opt.has_value());
  EXPECT_EQ(opt.value().x, 1);
  EXPECT_EQ(opt.value().y, 2);
}

// 测试赋值nullopt
TEST(OptionalTest, AssignNullopt) {
  optional<int> opt(5);
  opt = nullopt;
  EXPECT_FALSE(opt.has_value());
}

// 测试赋值值
TEST(OptionalTest, AssignValue) {
  optional<int> opt;
  opt = 10;
  EXPECT_TRUE(opt.has_value());
  EXPECT_EQ(opt.value(), 10);
}

// 测试拷贝赋值
TEST(OptionalTest, CopyAssignment) {
  optional<int> opt1(5);
  optional<int> opt2 = opt1;
  EXPECT_TRUE(opt2.has_value());
  EXPECT_EQ(opt2.value(), 5);
}

// 测试移动赋值
TEST(OptionalTest, MoveAssignment) {
  optional<int> opt1(5);
  optional<int> opt2 = std::move(opt1);
  EXPECT_TRUE(opt2.has_value());
  EXPECT_EQ(opt2.value(), 5);
}

// 测试emplace
TEST(OptionalTest, Emplace) {
  optional<std::string> opt;
  opt.emplace("Hello");
  EXPECT_TRUE(opt.has_value());
  EXPECT_EQ(opt.value(), "Hello");
}

// 测试reset
TEST(OptionalTest, Reset) {
  optional<int> opt(10);
  opt.reset();
  EXPECT_FALSE(opt.has_value());
}

// 测试swap
TEST(OptionalTest, Swap) {
  optional<int> opt1(5), opt2(10);
  opt1.swap(opt2);
  EXPECT_EQ(opt1.value(), 10);
  EXPECT_EQ(opt2.value(), 5);
}

// 测试value_or
TEST(OptionalTest, ValueOr) {
  optional<int> opt;
  EXPECT_EQ(opt.value_or(42), 42);
  opt = 10;
  EXPECT_EQ(opt.value_or(42), 10);
}

// 测试访问无效值抛出异常
TEST(OptionalTest, AccessInvalidValue) {
  optional<int> opt;
  EXPECT_THROW(opt.value(), bad_optional_access);
}

// 测试and_then
TEST(OptionalTest, AndThen) {
  optional<int> opt(5);
  auto result = opt.and_then([](int x) { return optional<int>(x * 2); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

// 测试transform
TEST(OptionalTest, Transform) {
  optional<int> opt(5);
  auto result = opt.transform([](int x) { return x * 2; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

// 测试or_else
TEST(OptionalTest, OrElse) {
  optional<int> opt;
  auto result = opt.or_else([] { return optional<int>(42); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}
