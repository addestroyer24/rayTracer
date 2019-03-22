BUILD_TYPE="Debug"

if [[ $# -ge 1 ]] ; then
    BUILD_TYPE=$1
fi

[ -e build/ ] || mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build ./
