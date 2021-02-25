## xalwart.orm

### Requirements

C++ compiler minimum version:
* Ubuntu: g++-10

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

### Testing

Use valgrind to check for memory leaks:
```bash
valgrind --leak-check=full ./your-executable
```
