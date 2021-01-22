FROM ubuntu:20.04 as base

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends g++-10 make zip cmake git libsqlite3-dev zlib1g-dev \
	 libreadline-gplv2-dev libncursesw5-dev libssl-dev tk-dev libgdbm-dev libc6-dev libbz2-dev libffi-dev \
     python3.9 python3.9-dev python3.9-distutils build-essential curl ca-certificates


# Install pip for python 3.9
RUN curl https://bootstrap.pypa.io/get-pip.py -o get_pip.py
RUN python3.9 get_pip.py && python3.9 -m pip install pytest
SHELL ["/bin/bash", "-c"]


FROM base as build

WORKDIR /

# Download boost code.
RUN git clone --jobs 10 --recursive --branch boost-1.75.0  https://github.com/boostorg/boost



# Copy boost configuration, which is necessary for building python librs correctly.
COPY user-config.jam /boost/user-config.jam
RUN ls -la /boost
# Build boost, with python, in a static confgiuration so that we don't need to deploy it.
# Boost will install to /usr/local/lib
RUN cd boost && ./bootstrap.sh --with-python-version=3.9 && \
    ./b2 --toolset=gcc cxxflags="-fPIC -std=c++2a" --user-config=/boost/user-config.jam address-model=64 link=static runtime-link=static -j 20 threading=multi install && \
    ./b2 --toolset=gcc cxxflags="-fPIC -std=c++2a" --prefix=/install/boost --user-config=/boost/user-config.jam address-model=64 link=static runtime-link=static -j 20 threading=multi install 

# Only copy over boost install files--keeping source dirs is too much.
FROM base as final
COPY --from=build /install/boost /*
RUN git clone https://github.com/emscripten-core/emsdk.git /emsdk
RUN cd /emsdk && git pull && ./emsdk install sdk-upstream-master-64bit && ./emsdk activate sdk-upstream-master-64bit
RUN source /emsdk/emsdk_env.sh
