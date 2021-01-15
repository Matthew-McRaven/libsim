FROM gcc:9

run apt update && apt install -y cmake

COPY . /libsim

WORKDIR /libsim
RUN mkdir build && cd build && cmake .. && make && ./test/Test -v high