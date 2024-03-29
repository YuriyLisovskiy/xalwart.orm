#!/usr/bin/env bash

SYSTEM_NAME=$1
CC_NAME=$2
CC_VERSION=$3
DATABASE=$4
shift 4
DATABASE_BUILD_OPTIONS=$*

if [[ "${CC_NAME}" == "gcc" ]]; then
  CXX_NAME="g++"
elif [[ "${CC_NAME}" == "clang" ]]; then
  CXX_NAME="clang++"
else
  echo "Compiler is not supported: ${CC_NAME}" && exit 1
fi

# Install base library.
cd /app/xalwart.base-"${SYSTEM_NAME}"-"${CC_NAME}"-"${CC_VERSION}" || exit 1
cp -r include/. /usr/local/include
cp -r lib/. /usr/local/lib

mkdir -p /app/build
cd /app/build || exit 1

# Build the library.
if [[ "${SYSTEM_NAME}" == "alpine"* ]]; then
  apk update && apk upgrade
  apk add --update sqlite-libs sqlite-dev postgresql-libs postgresql-dev
  ldconfig /etc/ld.so.conf.d
  cmake -D CMAKE_C_COMPILER="${CC_NAME}" \
        -D CMAKE_CXX_COMPILER="${CXX_NAME}" \
        -D CMAKE_BUILD_TYPE=Release \
        ${DATABASE_BUILD_OPTIONS} \
        ..
elif [[ "${SYSTEM_NAME}" == "ubuntu"* ]]; then
  apt-get update && apt-get -y upgrade
  apt-get install -y sqlite3 libsqlite3-dev libpq-dev
  ldconfig
  cmake -D CMAKE_BUILD_TYPE=Release \
        ${DATABASE_BUILD_OPTIONS} \
        ..
else
  echo "System is not supported: ${SYSTEM_NAME}" && exit 1
fi

make xalwart.orm && make install

# Copy installed library to the result directory.
BUILD_PATH=/app/xalwart.orm-${DATABASE}-"${SYSTEM_NAME}"-"${CC_NAME}"-"${CC_VERSION}"
cd /usr/local || exit 1
mkdir -p "${BUILD_PATH}"/include
mkdir -p "${BUILD_PATH}"/lib
cd include/ || exit 1
cp -r xalwart.orm "${BUILD_PATH}"/include
cd ../lib/ || exit 1
cp libxalwart.orm* "${BUILD_PATH}"/lib
