#include "unique_ptr.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace MySTL;

// 测试默认构造函数
TEST(UniquePtrTest, DefaultConstructor) {
  unique_ptr<int> up;
  EXPECT_EQ(up.get(), nullptr);
  EXPECT_FALSE(up);
}

// 测试带原始指针的构造函数
TEST(UniquePtrTest, ConstructorWithRawPointer) {
  unique_ptr<int> up(new int(42));
  EXPECT_NE(up.get(), nullptr);
  EXPECT_EQ(*up, 42);
  EXPECT_TRUE(up);
}

// 测试移动构造函数
TEST(UniquePtrTest, MoveConstructor) {
  unique_ptr<int> up1(new int(42));
  unique_ptr<int> up2(std::move(up1));
  EXPECT_NE(up2.get(), nullptr);
  EXPECT_EQ(*up2, 42);
  EXPECT_EQ(up1.get(), nullptr);
}

// 测试移动赋值运算符
TEST(UniquePtrTest, MoveAssignmentOperator) {
  unique_ptr<int> up1(new int(42));
  unique_ptr<int> up2;
  up2 = std::move(up1);
  EXPECT_NE(up2.get(), nullptr);
  EXPECT_EQ(*up2, 42);
  EXPECT_EQ(up1.get(), nullptr);
}

// 测试 reset 函数
TEST(UniquePtrTest, ResetFunction) {
  unique_ptr<int> up(new int(42));
  EXPECT_NE(up.get(), nullptr);
  up.reset();
  EXPECT_EQ(up.get(), nullptr);
}

// 测试 release 函数
TEST(UniquePtrTest, ReleaseFunction) {
  unique_ptr<int> up(new int(42));
  int *raw_ptr = up.release();
  EXPECT_EQ(up.get(), nullptr);
  EXPECT_NE(raw_ptr, nullptr);
  EXPECT_EQ(*raw_ptr, 42);
  delete raw_ptr; // 手动释放原始指针
}

// 测试 swap 函数
TEST(UniquePtrTest, SwapFunction) {
  unique_ptr<int> up1(new int(42));
  unique_ptr<int> up2(new int(10));
  up1.swap(up2);
  EXPECT_EQ(*up1, 10);
  EXPECT_EQ(*up2, 42);
}

// 测试数组支持
TEST(UniquePtrTest, ArraySupport) {
  unique_ptr<int[]> up(new int[3]{1, 2, 3});
  EXPECT_EQ(up[0], 1);
  EXPECT_EQ(up[1], 2);
  EXPECT_EQ(up[2], 3);
  up[0] = 10;
  EXPECT_EQ(up[0], 10);
}

// 测试 make_unique
TEST(UniquePtrTest, MakeUnique) {
  auto up = make_unique<int>(42);
  EXPECT_NE(up.get(), nullptr);
  EXPECT_EQ(*up, 42);
}

// 测试 make_unique 数组
TEST(UniquePtrTest, MakeUniqueArray) {
  auto up = make_unique<int[]>(3);
  up[0] = 1;
  up[1] = 2;
  up[2] = 3;
  EXPECT_EQ(up[0], 1);
  EXPECT_EQ(up[1], 2);
  EXPECT_EQ(up[2], 3);
}

// 测试 get_deleter
struct CustomDeleter {
  void operator()(int *ptr) const { delete ptr; }
};

TEST(UniquePtrTest, GetDeleter) {
  unique_ptr<int, CustomDeleter> up(new int(42));
  auto deleter = up.get_deleter();
  EXPECT_NE(up.get(), nullptr);
  deleter(up.release()); // 手动删除原始指针
}

// 测试布尔转换操作符
TEST(UniquePtrTest, BoolConversion) {
  unique_ptr<int> up;
  EXPECT_FALSE(up);
  up.reset(new int(42));
  EXPECT_TRUE(up);
}
