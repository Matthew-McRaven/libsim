mkdir -p build
cd build

(
	source /emsdk/emsdk_env.sh
	mkdir -p js
	cd js
	emcmake cmake -DBoost_INCLUDE_DIR=/boost/ ../..
	emmake make -j$(nproc)
	node test/libisa/test-lib.js
)