mkdir -p build-depends
cd build-depends
(
	git clone --jobs 30 https://github.com/emscripten-core/emsdk.git emsdk
	cd emsdk
	git pull
	./emsdk install sdk-upstream-master-64bit -j30
	./emsdk activate sdk-upstream-master-64bit -j30
	echo "hi"
)

(
	git clone --jobs 30 --recursive --branch boost-1.75.0  https://github.com/boostorg/boost boost
	cd boost
	./bootstrap.sh
	./b2 --link=static --runtime-link=static --stagedir=./stage-native -j30
	ls -la .
	source ../emsdk/emsdk_env.sh 
	ls -la .
	emconfigure ./b2 --toolset=gcc --link=static --runtime-link=static --build-dir=./build-js -j30
	emconfigure ./b2 --toolset=gcc --link=static --runtime-link=static --stagedir=./stage-js --build-dir=./build-js -j30
)