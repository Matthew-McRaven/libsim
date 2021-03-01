
mkdir -p build
cd build

(
	source /emsdk/emsdk_env.sh
	mkdir -p native
	cd native
	cmake ../.. -DCMAKE_C_COMPILER=$(which clang) -DCMAKE_CXX_COMPILER=$(which clang++)
	make -j$(nproc) && make test
)
