#!/usr/bin/env bash

set -e

COMPILER=$1

if [[ "${COMPILER}" == "clang" ]]; then
	apk add --update --no-cache clang clang-dev
elif [[ "${COMPILER}" == "gcc" ]]; then
	apk add --update --no-cache gcc
else
	echo "Compiler is not supported: ${COMPILER}" >&2
  exit 1
fi
