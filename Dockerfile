FROM ubuntu:20.04
run apt-get update && apt-get install -y gcc 
run apt-get update && apt-get install -y g++
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin
run apt-get update && apt-get install -y  libcfitsio-dev  make libmagickwand-6.q16-6 libmagickwand-6.q16-dev libmagick++-6.q16-dev

run mkdir /code
COPY ./SPoCA /code/SPoCA
ENV PATH=$PATH:/usr/lib/x86_64-linux-gnu/ImageMagick-6.9.10/bin-q16/
RUN cd /code/SPoCA && ./makemake.sh > Makefile && make -j 8
ENV LD_LIBRARY_PATH=LD_LIBRARY_PATH:/code/SPoCA/lib
