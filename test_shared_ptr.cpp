#include "shared_ptr.hpp"
#include <iostream>
#include <memory>

struct MyClass {
  int value;
  MyClass(int v) : value(v) {
    std::cout << "MyClass(" << value << ") created.\n";
  }
  ~MyClass() { std::cout << "MyClass(" << value << ") destroyed.\n"; }
};

void test_shared_ptr_basic() {
  std::cout << "Testing shared_ptr basic functionality\n";

  // 创建 shared_ptr
  MyClass *ptr = new MyClass(42);
  MySTL::shared_ptr<MyClass> sp1(ptr);
  std::cout << "use_count after construction: " << sp1.use_count() << "\n";

  // 拷贝构造
  MySTL::shared_ptr<MyClass> sp2 = sp1;
  std::cout << "use_count after copy: " << sp1.use_count() << "\n";

  // 移动构造
  MySTL::shared_ptr<MyClass> sp3 = std::move(sp2);
  std::cout << "use_count after move: " << sp1.use_count() << "\n";

  // 重置 shared_ptr
  sp1.reset();
  std::cout << "use_count after reset: " << sp1.use_count() << "\n";
}

void test_make_shared() {
  std::cout << "Testing make_shared\n";

  // 使用 make_shared 创建 shared_ptr
  auto sp1 = MySTL::make_shared<MyClass>(10);
  std::cout << "use_count after make_shared: " << sp1.use_count() << "\n";

  // 测试其他类型的对象
  auto sp3 = MySTL::make_shared<int>(42);
  std::cout << "use_count for int: " << sp3.use_count() << "\n";
}

void test_owner_comparison() {
  std::cout << "Testing owner_before and owner_equal\n";

  auto sp1 = MySTL::make_shared<int>(100);
  auto sp2 = MySTL::make_shared<int>(200);

  if (sp1.owner_before(sp2)) {
    std::cout << "sp1's owner is before sp2's owner.\n";
  } else {
    std::cout << "sp1's owner is NOT before sp2's owner.\n";
  }

  if (sp1.owner_equal(sp2)) {
    std::cout << "sp1's owner is the same as sp2's owner.\n";
  } else {
    std::cout << "sp1's owner is NOT the same as sp2's owner.\n";
  }
}

int main() {
  test_shared_ptr_basic();
  test_make_shared();
  test_owner_comparison();

  return 0;
}
