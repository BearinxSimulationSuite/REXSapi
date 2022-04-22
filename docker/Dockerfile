FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get -y install g++ && \
    apt-get -y install cmake && \
    apt-get -y install git && \
    apt-get -y install ninja-build && \ 
    apt-get -y install lcov && \
    apt-get -y autoremove && \
    apt-get -y clean

COPY build.sh /
WORKDIR /build

RUN chmod +x /build.sh
ENTRYPOINT ["/build.sh"]
