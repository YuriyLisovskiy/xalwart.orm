## xalwart.orm

[![gcc](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-gcc.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-gcc.yml)
[![clang](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-clang.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-clang.yml)

### Requirements

- C++ compiler:
  - `g++`: v10 or later.
  - `clang++`: v10 or later.
- CMake: `2.4` or later.
- Additional libraries:
  - [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core)

### Available drivers:
* `sqlite3`:
    ```bash
    sudo apt-get install sqlite3 libsqlite3-dev
    ```
  To enable this driver, add the next line into `CMakeLists.txt`
  before building an application:
  ```cmake
  add_compile_definitions(USE_SQLITE3)
  ```

### Build and Install
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

### Testing

Use valgrind to check for memory leaks:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
valgrind --leak-check=full ./tests/unittests-all
```
