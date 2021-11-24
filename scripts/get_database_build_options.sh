#!/usr/bin/env bash

DATABASE=$1

SQLITE_OPTIONS=""
POSTGRESQL_OPTIONS=""

if [[ "$DATABASE" == *"sqlite"* ]]; then
  SQLITE_OPTIONS="-D XW_USE_SQLITE3=yes "
fi
if [[ "$DATABASE" == *"postgresql"* ]]; then
  POSTGRESQL_OPTIONS="-D XW_USE_POSTGRESQL=yes"
fi

echo "${SQLITE_OPTIONS}${POSTGRESQL_OPTIONS}"
