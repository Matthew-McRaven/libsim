ARG BUILD_VERSION="0.3.3"
FROM registry.gitlab.com/pep10/libsim/emboost:$BUILD_VERSION

COPY . /libsim

WORKDIR /libsim