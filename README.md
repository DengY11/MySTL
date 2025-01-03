# MySTL

c++20标准编写的STL，目前已已实现了unique_ptr, shared_ptr, optional ,functional

采用google测试框架

测试文件构建方法：

```bash
rm -rf build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build
cmake --build build
```

运行:

```bash
./test_optional
./test_unique_ptr
./test_shared_ptr
./test_functional

```
