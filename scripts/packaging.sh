#!/usr/bin/env bash

CUR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_PATH="$CUR_PATH/.."
BUILD_PATH="$CUR_PATH/../build"
PROJECT_NAME="account-manager"

cd $ROOT_PATH
mkdir -p $BUILD_PATH
cd $BUILD_PATH

if [ "$PROJECT" == "telstra" ]; then
  cmake -DCMAKE_BUILD_TYPE=Release -DPROJECT=$PROJECT .. || { echo "cmake failed"; exit 1; }
elif [ "$PROJECT" == "bi" ]; then
  cmake -DCMAKE_BUILD_TYPE=Release -DPROJECT=$PROJECT .. || { echo "cmake failed"; exit 1; }
else
  cmake -DCMAKE_BUILD_TYPE=Release .. || { echo "cmake failed"; exit 1; }
fi

make -j$(nproc) -l$(nproc) || { echo "make failed"; exit 1; }
make install


cd $ROOT_PATH
cp -aR packaging/qtifw/packages/$PROJECT_NAME $PROJECT_NAME
mkdir $PROJECT_NAME/data
mv outputs $PROJECT_NAME/data/$PROJECT_NAME
tar cvzf $PROJECT_NAME.tar.gz $PROJECT_NAME
rm -rf $PROJECT_NAME
