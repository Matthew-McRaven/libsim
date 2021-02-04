ARG BUILD_VERSION="0.3.2"
FROM registry.gitlab.com/pep10/libsim/pyboost:$BUILD_VERSION

COPY . /libsim

WORKDIR /libsim