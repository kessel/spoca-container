#!/bin/bash
git clone git@github.com:kessel/SPoCA.git
docker build --tag spoca --progress=plain .
