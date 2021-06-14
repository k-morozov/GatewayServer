FROM ubuntu:focal
MAINTAINER Konstantin Morozov <morozov-kst@yandex.ru>
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
        build-essential make autoconf cmake git pkg-config \
        automake libtool curl unzip \
        gcc-10 g++-10 \
        libpq-dev \
        python3-pip && \
        apt-get clean && \
        pip3 install conan

COPY Messaging-framework-0.1.3-Linux.deb .
RUN apt-get install ./Messaging-framework-0.1.3-Linux.deb