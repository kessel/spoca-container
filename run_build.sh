#!/bin/bash
usr="--user $(id -u $USER):$(id -g $USER)"

docker run -it $usr -e HOME -v ${PWD}/code:/code -w /code spoca \
    bash -c 'cd SPoCA && export PATH=$PATH:`find /usr/ -name Magick++-config | xargs dirname` && echo $PATH && ./makemake.sh > Makefile && make -j 1 ' --name spoca
