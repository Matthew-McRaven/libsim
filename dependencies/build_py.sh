git clone --branch v3.8.5 https://github.com/python/cpython
cd cpython && ./configure --enable-optimizations && make -j 20 && make install DESTDIR=../build/python3
