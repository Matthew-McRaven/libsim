FROM ubuntu:20.04
ARG boost_version=1.75.0
# Installing git installs tzdata, and we don't want to be forced to interact with the build.
ENV DEBIAN_FRONTEND=noninteractive 
RUN apt-get update && apt-get install -y g++ make zip cmake git libsqlite3-dev zlib1g-dev libreadline-gplv2-dev libncursesw5-dev libssl-dev tk-dev libgdbm-dev libc6-dev libbz2-dev libffi-dev

# Download python
# If you want to change the python version, be sure to modify the boost bootstrap command AND user-config.jam
RUN git clone --branch v3.8.5 https://github.com/python/cpython
RUN cd cpython && ./configure --enable-optimizations && make -j 20 && make install && make install DESTDIR=/install/py

# Download boost code.
RUN git clone --jobs 10 --recursive --branch boost-${boost_version}  https://github.com/boostorg/boost
# Copy boost configuration, which is necessary for building python librs correctly.
COPY user-config.jam boost/
# Build boost, with python, in a static confgiuration so that we don't need to deploy it.
# Boost will install to /usr/local/lib
RUN cd boost && ./bootstrap.sh --with-python-version=3.8 && \
    ./b2 cxxflags="-fPIC -std=c++17" --user-config=/boost/user-config.jam address-model=64 link=static runtime-link=static -j 20 threading=multi install && \
    ./b2 cxxflags="-fPIC -std=c++17" --prefix=/install/boost --user-config=/boost/user-config.jam address-model=64 link=static runtime-link=static -j 20 threading=multi install 
