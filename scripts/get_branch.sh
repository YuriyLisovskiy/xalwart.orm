#!/bin/sh

BRANCH_NAME=$1

if [ "$BRANCH_NAME" = "master" ]; then
  echo "master"
else
  echo "dev"
fi
