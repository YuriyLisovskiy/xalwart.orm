## xalwart.orm

### Requirements

C++ compiler minimum version:
* Ubuntu: g++-10

### Available drivers:
* `sqlite3`:
    ```
    $ sudo apt-get install sqlite3 libsqlite3-dev
    ```

### Testing

Use valgrind to check for memory leaks:
```
$ valgrind --leak-check=full ./your-executable
```
