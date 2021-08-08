
mkdir -p build
cd build

(
	source /emsdk/emsdk_env.sh
	mkdir -p coverage 
	cd coverage
	cmake ../.. -DCMAKE_C_COMPILER=$(which clang) -DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_BUILD_TYPE=Debug -Dcode_coverage=1
	make -j$(nproc) && make test -j$(nproc)
)
