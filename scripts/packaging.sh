#!/usr/bin/env bash

CUR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_PATH="$CUR_PATH/.."
PROJECT_NAME="account-manager"

make || { echo "make failed"; exit 1; }

cd $ROOT_PATH
cp -aR packaging/qtifw/packages/$PROJECT_NAME $PROJECT_NAME
mkdir $PROJECT_NAME/data
mv outputs $PROJECT_NAME/data/$PROJECT_NAME
tar cvzf $PROJECT_NAME.tar.gz $PROJECT_NAME
rm -rf $PROJECT_NAME
