mkdir -p build
cd build

(
	mkdir -p native
	cd native
	cmake ../..
	make -j30
	make test
)

(
	source ../build-depends/emsdk/emsdk_env.sh
	mkdir -p js
	cd js
	BOOST_ROOT=../build-depends/boost/stage-js emcmake cmake ../..
	emmake make -j30
)