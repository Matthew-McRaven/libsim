mkdir -p build
cd build

(
	source /emsdk/emsdk_env.sh
	mkdir -p js
	cd js
	emcmake cmake -DBoost_INCLUDE_DIR=/boost/ ../.. -DCMAKE_BUILD_TYPE=RelWithDebInfo
	emmake make -j$(nproc)
	emmake make test -j$(nproc)
)