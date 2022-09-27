CUR_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_PATH=$CUR_PATH/../build

PROJECT="$1"

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

