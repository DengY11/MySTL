#include <gtest/gtest.h>
#include "include/functional.hpp"

using namespace MySTL;

// 测试 MySTL::function
TEST(FunctionTest, EmptyFunction
) {
function<void()> f;
EXPECT_FALSE(f);
EXPECT_THROW(f(), std::bad_function_call
);
}

TEST(FunctionTest, AssignAndCall
) {
int x = 0;
function<void()> f = [&x]() { x = 42; };
EXPECT_TRUE(f);
f();
EXPECT_EQ(x,
42);
}

TEST(FunctionTest, CopyFunction
) {
int x = 0;
function<void()> f1 = [&x]() { x = 100; };
function<void()> f2 = f1;
f2();
EXPECT_EQ(x,
100);
}

TEST(FunctionTest, MoveFunction
) {
int x = 0;
function<void()> f1 = [&x]() { x = 200; };
function<void()> f2 = std::move(f1);
EXPECT_FALSE(f1);
EXPECT_TRUE(f2);
f2();
EXPECT_EQ(x,
200);
}

TEST(FunctionTest, TargetType
) {
function<int(int)> f = [](int a) { return a * 2; };
EXPECT_EQ(f
.
target_type(),
typeid(decltype(f)));
}

TEST(FunctionTest, SwapFunction
) {
function<void()> f1 = []() {};
function<void()> f2 = []() {};
f1.
swap(f2);
EXPECT_TRUE(f1);
EXPECT_TRUE(f2);
}

// 测试 MySTL::move_only_function
TEST(MoveOnlyFunctionTest, EmptyFunction
) {
move_only_function<void()> f;
EXPECT_FALSE(f);
// 不能复制，只能测试是否为空
}

TEST(MoveOnlyFunctionTest, AssignAndCall
) {
int x = 0;
move_only_function<void()> f = [&x]() { x = 42; };
EXPECT_TRUE(f);
f();
EXPECT_EQ(x,
42);
}

TEST(MoveOnlyFunctionTest, MoveFunction
) {
int x = 0;
move_only_function<void()> f1 = [&x]() { x = 200; };
move_only_function<void()> f2 = std::move(f1);
EXPECT_FALSE(f1);
EXPECT_TRUE(f2);
f2();
EXPECT_EQ(x,
200);
}

TEST(MoveOnlyFunctionTest, InPlaceConstruction
) {
int x = 0;
move_only_function<void()> f(std::in_place_type<std::function<void()>>,[&x]() { x = 300; }
);
EXPECT_TRUE(f);
f();
EXPECT_EQ(x,
300);
}

TEST(MoveOnlyFunctionTest, SwapFunction
) {
move_only_function<void()> f1 = []() {};
move_only_function<void()> f2 = []() {};
f1.
swap(f2);
EXPECT_TRUE(f1);
EXPECT_TRUE(f2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
