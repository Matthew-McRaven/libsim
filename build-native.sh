
mkdir -p build
cd build

(
	source /emsdk/emsdk_env.sh
	CXX=$(which clang++)
	mkdir -p native
	cd native
	cmake ../.. -DCMAKE_BUILD_TYPE=Debug
	make -j30 && make test
)
