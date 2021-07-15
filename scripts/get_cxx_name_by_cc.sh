#!/usr/bin/env bash

set -e

COMPILER=$1

if [[ "${COMPILER}" == "clang" ]]; then
	echo -n "clang++"
elif [[ "${COMPILER}" == "gcc" ]]; then
	echo -n "g++"
else
	echo "Unknown compiler: ${COMPILER}" >&2
  exit 1
fi
