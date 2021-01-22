# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.154.2/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="buster"
ARG boost_version="1.75.0"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends g++-10 make zip cmake git libsqlite3-dev zlib1g-dev \
	 libreadline-gplv2-dev libncursesw5-dev libssl-dev tk-dev libgdbm-dev libc6-dev libbz2-dev libffi-dev \
     python3.9 python3.9-dev

ENV CXX=/usr/bin/g++-10



WORKDIR /

# Download boost code.
RUN git clone --jobs 10 --recursive --branch boost-1.75.0  https://github.com/boostorg/boost && git clone https://github.com/emscripten-core/emsdk.git /emsdk
# Copy boost configuration, which is necessary for building python librs correctly.
COPY user-config.jam /boost/user-config.jam
RUN ls -la /boost
# Build boost, with python, in a static confgiuration so that we don't need to deploy it.
# Boost will install to /usr/local/lib
RUN cd boost && ./bootstrap.sh --with-python-version=3.9 && \
    ./b2 cxxflags="-fPIC -std=c++17" --user-config=/boost/user-config.jam address-model=64 link=static runtime-link=static -j 20 threading=multi install && \
    ./b2 cxxflags="-fPIC -std=c++17" --prefix=/install/boost --user-config=/boost/user-config.jam address-model=64 link=static runtime-link=static -j 20 threading=multi install 

RUN cd /emsdk && git pull && ./emsdk install sdk-upstream-master-64bit && ./emsdk activate sdk-upstream-master-64bit

SHELL ["/bin/bash", "-c"]
RUN source /emsdk/emsdk_env.sh