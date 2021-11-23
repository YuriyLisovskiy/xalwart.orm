## xalwart.orm
[![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/)
[![cmake](https://img.shields.io/badge/cmake-%3E=2.8.12-success)](https://cmake.org/)
[![alpine](https://img.shields.io/badge/Alpine_Linux-0D597F?style=flat&logo=alpine-linux&logoColor=white)](https://alpinelinux.org/)
[![ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=flat&logo=ubuntu&logoColor=white)](https://ubuntu.com/)
[![macOS](https://img.shields.io/badge/macOS-343D46?style=flat&logo=apple&logoColor=F0F0F0)](https://www.apple.com/macos)

### Build Status
| @ | Build |
|---|---|
| Dev branch: | [![CI](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/test.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/test.yml?query=branch%3Adev) |
| Master branch: | [![CI](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/test.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.orm/actions/workflows/test.yml?query=branch%3Amaster) |

## Requirements
The following compilers are tested with the CI system, and are known to work on:

Alpine Linux and Ubuntu:
* g++ 10 or later
* clang++ 10 or later

macOS:
* clang++ 12 or later

To build the library from source CMake 3.12 or later is required.

### Dependencies
The following library is required:
- [xalwart.base](https://github.com/YuriyLisovskiy/xalwart.base) 0.0.0 or later

Available drivers:
* `sqlite3` (`SQLITE3`):
    ```bash
    # Ubuntu
    sudo apt-get install sqlite3 libsqlite3-dev
    
    # macOS (if not installed by default)
    brew install sqlite3
    ```
  Enable it in `CMakeLists.txt`:
  ```cmake
  add_compile_definitions(USE_SQLITE3)
  ```
* `libpq` 14.0 or later (`POSTGRESQL`):
    ```bash
    # Ubuntu
    sudo apt-get install libpq-dev
  
    # macOS
    brew install libpq
    ```
  Enable it in `CMakeLists.txt`:
  ```cmake
  add_compile_definitions(USE_POSTGRESQL)
  ```

## Compile from Source
* `BUILD_SHARED_LIBS`: build a shared or static library (`ON` by default).
* `LIBRARY_ROOT`: installation directory root (`/usr/local` by default).
* `LIBRARY_INCLUDE_DIR`: include installation directory (`${LIBRARY_ROOT}/include` by default).
* `LIBRARY_LINK_DIR`: library installation directory (`${LIBRARY_ROOT}/lib` by default).
* `XW_USE_DB_DRIVER_NAME`: the name of a driver that will be used in ORM.
  `DB_DRIVER_NAME` should be replaced by one of the available drivers shown
  in [dependencies](#dependencies), example: `XW_USE_SQLITE3`.

PostgreSQL-specific arguments:
* `PostgreSQL_ROOT`: root directory for PostgreSQL library in case of non-standard installation path.
```bash
git clone https://github.com/YuriyLisovskiy/xalwart.orm.git
cd xalwart.orm
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release \
      -D XW_USE_DB_DRIVER_NAME=ON \
      ..
make xalwart.orm && make install
```

## Testing
```bash
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Debug \
      -D XW_CONFIGURE_TESTS=ON \
      ..
make unittests-all
valgrind --leak-check=full ./tests/unittests-all
```
