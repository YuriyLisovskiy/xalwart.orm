FROM alpine:3.13

ARG cc=clang
ARG cxx=clang++
ARG build_shared_lib=off
ARG GH_ACCESS_TOKEN

RUN echo "ipv6" >> /etc/modules

RUN apk update && apk upgrade

RUN apk add --update --no-cache bash

COPY . .

RUN bash ./scripts/install_compiler_alpine.sh $cc

RUN apk add --update --no-cache \
    alpine-sdk \
    dpkg \
    cmake \
    ccache \
    git \
    valgrind

RUN git clone -q https://github.com/google/googletest.git /googletest && \
    mkdir -p /googletest/build && \
    cd /googletest/build && \
    cmake -DCMAKE_C_COMPILER=$cc \
          -DCMAKE_CXX_COMPILER=$cxx \
          .. && \
    make && make install && \
    cd / && rm -rf /googletest

RUN git clone -q https://$GH_ACCESS_TOKEN@github.com/YuriyLisovskiy/xalwart.base.git /xalwart.base && \
    mkdir -p /xalwart.base/build && \
    cd /xalwart.base && \
    git checkout dev && \
    cd /xalwart.base/build && \
    cmake -DCMAKE_C_COMPILER=$cc \
          -DCMAKE_CXX_COMPILER=$cxx \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIB=$build_shared_lib \
          .. && \
    make && make install && \
    cd / && rm -rf /xalwart.base

RUN ldconfig /etc/ld.so.conf.d

RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_C_COMPILER=$cc \
          -DCMAKE_CXX_COMPILER=$cxx \
          -DCMAKE_BUILD_TYPE=Debug \
          -DBUILD_SHARED_LIB=$build_shared_lib \
          .. && \
    make unittests-all

CMD valgrind --leak-check=full $APP_HOME/build/tests/unittests-all
