## xalwart.orm

### Requirements

C++ compiler minimum version:
* Ubuntu: g++-10

### Available drivers:
* `sqlite3`:
    ```bash
    sudo apt-get install sqlite3 libsqlite3-dev
    ```
  To enable this driver, add `-DUSE_SQLITE3=yes` argument when
  configuring build with `cmake`.

### Testing

Use valgrind to check for memory leaks:
```bash
valgrind --leak-check=full ./your-executable
```
