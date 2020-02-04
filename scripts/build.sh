#!/bin/bash

CUR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_PATH="$CUR_PATH/.."
PROJECT_NAME="account-manager"
PROJECT=$1

if [ "$PROJECT" == "telstra" ]; then
  make TELSTRA=1 || { echo "make failed"; exit 1; }
else
  make || { echo "make failed"; exit 1; }
fi
