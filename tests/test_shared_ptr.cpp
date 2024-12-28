#include "include/shared_ptr.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace MySTL;

// 测试默认构造函数
TEST(SharedPtrTest, DefaultConstructor
) {
shared_ptr<int> sp;
EXPECT_EQ(sp
.
get(),
nullptr);
EXPECT_EQ(sp
.
use_count(),
0);
EXPECT_FALSE(sp);
}

// 测试原始指针构造函数
TEST(SharedPtrTest, ConstructorWithRawPointer
) {
shared_ptr<int> sp(new int(42));
EXPECT_NE(sp
.
get(),
nullptr);
EXPECT_EQ(*sp,
42);
EXPECT_EQ(sp
.
use_count(),
1);
EXPECT_TRUE(sp);
}

// 测试拷贝构造函数
TEST(SharedPtrTest, CopyConstructor
) {
shared_ptr<int> sp1(new int(42));
shared_ptr<int> sp2(sp1);
EXPECT_EQ(sp1
.
get(), sp2
.
get()
);
EXPECT_EQ(sp1
.
use_count(),
2);
EXPECT_EQ(sp2
.
use_count(),
2);
}

// 测试移动构造函数
TEST(SharedPtrTest, MoveConstructor
) {
shared_ptr<int> sp1(new int(42));
shared_ptr<int> sp2(std::move(sp1));
EXPECT_EQ(sp2
.
use_count(),
1);
EXPECT_EQ(*sp2,
42);
EXPECT_EQ(sp1
.
get(),
nullptr);
EXPECT_EQ(sp1
.
use_count(),
0);
}

// 测试拷贝赋值运算符
TEST(SharedPtrTest, CopyAssignmentOperator
) {
shared_ptr<int> sp1(new int(42));
shared_ptr<int> sp2;
sp2 = sp1;
EXPECT_EQ(sp1
.
get(), sp2
.
get()
);
EXPECT_EQ(sp1
.
use_count(),
2);
EXPECT_EQ(sp2
.
use_count(),
2);
}

// 测试移动赋值运算符
TEST(SharedPtrTest, MoveAssignmentOperator
) {
shared_ptr<int> sp1(new int(42));
shared_ptr<int> sp2;
sp2 = std::move(sp1);
EXPECT_EQ(sp2
.
use_count(),
1);
EXPECT_EQ(*sp2,
42);
EXPECT_EQ(sp1
.
get(),
nullptr);
EXPECT_EQ(sp1
.
use_count(),
0);
}

// 测试 reset 函数
TEST(SharedPtrTest, ResetFunction
) {
shared_ptr<int> sp(new int(42));
EXPECT_EQ(sp
.
use_count(),
1);
sp.
reset();
EXPECT_EQ(sp
.
get(),
nullptr);
EXPECT_EQ(sp
.
use_count(),
0);
}

// 测试 use_count 和 unique
TEST(SharedPtrTest, UseCountAndUnique
) {
shared_ptr<int> sp(new int(42));
EXPECT_EQ(sp
.
use_count(),
1);
EXPECT_TRUE(sp
.
unique()
);
shared_ptr<int> sp2(sp);
EXPECT_EQ(sp
.
use_count(),
2);
EXPECT_FALSE(sp
.
unique()
);
}

// 测试数组支持
TEST(SharedPtrTest, ArraySupport
) {
shared_ptr<int[]> sp(new int[3]{1, 2, 3});
EXPECT_EQ(sp
.
get()[0],
1);
EXPECT_EQ(sp
.
get()[1],
2);
EXPECT_EQ(sp
.
get()[2],
3);
sp.
get()[0] = 10;
EXPECT_EQ(sp
.
get()[0],
10);
}

// 测试 enable_shared_from_this
struct MyClass : public enable_shared_from_this<MyClass> {
  int value;
  MyClass(int v) : value(v) {}
};

// 测试 make_shared
TEST(SharedPtrTest, MakeShared
) {
auto sp = make_shared < int > (42);
EXPECT_EQ(*sp,
42);
EXPECT_EQ(sp
.
use_count(),
1);
}

// 测试 static_pointer_cast
TEST(SharedPtrTest, StaticPointerCast
) {
shared_ptr<void> sp = make_shared < int > (42);
auto spInt = static_pointer_cast<int>(sp);
EXPECT_EQ(*spInt,
42);
EXPECT_EQ(sp
.
use_count(), spInt
.
use_count()
);
}

// 测试 dynamic_pointer_cast
TEST(SharedPtrTest, DynamicPointerCast
) {
struct Base {
  virtual ~Base() = default;
};
struct Derived : public Base {
  int value;
  Derived(int v) : value(v) {}
};

shared_ptr<Base> spBase = make_shared < Derived > (42);
auto spDerived = dynamic_pointer_cast<Derived>(spBase);
EXPECT_EQ(spDerived
->value, 42);
EXPECT_EQ(spBase
.
use_count(), spDerived
.
use_count()
);
}

// 测试 const_pointer_cast
TEST(SharedPtrTest, ConstPointerCast
) {
shared_ptr<const int> spConst = make_shared < int > (42);
auto spNonConst = const_pointer_cast<int>(spConst);
*
spNonConst = 10;
EXPECT_EQ(*spConst,
10);
}

// 测试 reinterpret_pointer_cast
TEST(SharedPtrTest, ReinterpretPointerCast
) {
shared_ptr<int> sp = make_shared < int > (42);
auto spReinterpret = reinterpret_pointer_cast<void>(sp);
EXPECT_EQ(sp
.
use_count(), spReinterpret
.
use_count()
);
}
