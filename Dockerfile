FROM gcc:10

RUN apt update && apt install -y cmake

COPY . /libsim

WORKDIR /libsim

RUN mkdir build && cd build && cmake .. && make -j20 && ./test/Test -v high