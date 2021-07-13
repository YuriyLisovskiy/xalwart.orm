## xalwart.orm
[![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/)
[![cmake](https://img.shields.io/badge/cmake-%3E=2.8.12-success)](https://cmake.org/)
[![alpine](https://img.shields.io/badge/Alpine_Linux-0D597F?style=flat&logo=alpine-linux&logoColor=white)](https://alpinelinux.org/)
[![ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=flat&logo=ubuntu&logoColor=white)](https://ubuntu.com/)

### Build Status
| @ | Build |
|---|---|
| Dev branch: | [![CI](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/ci.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/ci.yml?query=branch%3Adev) |
| Master branch: | [![CI](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/ci.yml?query=branch%3Amaster) |

## Requirements
The following compilers are tested with the CI system, and are known to work
on Alpine Linux and Ubuntu.
* g++ 10 or later
* clang++ 10 or later

To build the library from source CMake 2.8.12 or later is required.

### Dependencies
The following library is required:
- [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core) 0.x.x or later

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

## Compile from Source
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_DRIVER_NAME=yes ..
make

# for linux:
sudo make install
```

## Testing
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make unittests-all
```

Use valgrind to check for memory leaks:
```bash
valgrind --leak-check=full ./tests/unittests-all
```
