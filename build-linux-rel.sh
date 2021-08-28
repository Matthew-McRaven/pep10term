
mkdir -p build
cd build

(
	source /emsdk/emsdk_env.sh
	mkdir -p linux
	cd linux
	cmake ../.. -DCMAKE_C_COMPILER=$(which clang) -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_BUILD_TYPE=RelWithDebInfo
	make -j$(nproc) && make -j$(nproc) test
)
