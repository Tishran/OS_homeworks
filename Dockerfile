FROM ubuntu:22.04

RUN apt-get update

RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

RUN apt-get install -y software-properties-common

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test

RUN apt-get update

RUN apt-get install -y \
        make \
        build-essential \
        g++-13 \
        gcc-multilib \
        gdb \
        python3 \
        python3-pip \
        git \
        man \
        vim

ENV TZ=Europe/Moscow

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

ENV CXX="/usr/bin/g++-13"
ENV CC="/usr/bin/gcc-13"

RUN update-alternatives \
    --install /usr/bin/gcc gcc /usr/bin/gcc-13 20 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-13

WORKDIR /workspace
