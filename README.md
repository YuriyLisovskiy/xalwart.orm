## xalwart.orm [![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/)

| [![ubuntu](https://img.shields.io/static/v1?message=Ubuntu&logo=ubuntu&labelColor=ef4800&color=5c5c5c&logoColor=white&label=%20)](https://ubuntu.com/) | [![windows](https://img.shields.io/static/v1?message=Windows&logo=windows&labelColor=blue&color=5c5c5c&logoColor=white&label=%20)](https://www.microsoft.com/en-us/windows/) |
|:---:|:---:|
| [![clang](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-clang-ubuntu.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-clang-ubuntu.yml) | *not supported* |
| [![gcc](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-gcc-ubuntu.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/tests-gcc-ubuntu.yml) | *not supported* |

### Requirements

- C++ compiler:
  - `gcc`: v10 or later.
  - `clang`: v10 or later.
- CMake: `2.4` or later.
- Additional libraries:
  - [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core) (required)

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
