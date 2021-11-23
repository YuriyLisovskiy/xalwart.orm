#!/usr/bin/env bash

SYSTEM_NAME=$1
CC_NAME=$2
CC_VERSION=$3

if [[ "${CC_NAME}" == "gcc" ]]; then
  CXX_NAME="g++"
elif [[ "${CC_NAME}" == "clang" ]]; then
  CXX_NAME="clang++"
else
  echo "Compiler is not supported: ${CC_NAME}" && exit 1
fi

function install_lib {
  # Install the library.
  cd /app/xalwart."$1"-"${SYSTEM_NAME}"-"${CC_NAME}"-"${CC_VERSION}" || exit 1
  cp -r include/. /usr/local/include
  cp -r lib/. /usr/local/lib
}

# Install base library.
install_lib "base"

# Install orm library.
install_lib "orm"

# Update linker cache.
ldconfig /etc/ld.so.conf.d

mkdir -p /app/build
cd /app/build || exit 1
if [[ "${SYSTEM_NAME}" == "alpine"* ]]; then
  apk update && apk upgrade
  apk add --update sqlite-libs sqlite-dev postgresql-libs postgresql-dev
  ldconfig /etc/ld.so.conf.d
  cmake -D CMAKE_C_COMPILER="${CC_NAME}" \
        -D CMAKE_CXX_COMPILER="${CXX_NAME}" \
        -D CMAKE_BUILD_TYPE=Release \
        -D XW_USE_POSTGRESQL=yes \
        -D XW_USE_SQLITE3=yes \
        -D XW_CONFIGURE_LIB=OFF \
        -D XW_CONFIGURE_TESTS=ON \
        ..
elif [[ "${SYSTEM_NAME}" == "ubuntu"* ]]; then
  apt-get install sqlite3 libsqlite3-dev libpq-dev
  ldconfig
  cmake -D CMAKE_BUILD_TYPE=Release \
        -D XW_CONFIGURE_LIB=OFF \
        -D XW_CONFIGURE_TESTS=ON \
        ..
else
  echo "System is not supported: ${SYSTEM_NAME}" && exit 1
fi
make unittests-all

# Run tests using valgrind.
valgrind --leak-check=full ./tests/unittests-all
