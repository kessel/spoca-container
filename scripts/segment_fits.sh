#!/bin/bash
set -e
input=$1
output=$2
WORKDIR=`mktemp -d`
scriptdir="$( dirname -- "$BASH_SOURCE"; )";
CONFIGDIR=$scriptdir/../configs

/code/SPoCA/bin1/classification.x --config ${CONFIGDIR}/ch_classification.config ${input} --output ${WORKDIR} && \
/code/SPoCA/bin1/attribution.x --config ${CONFIGDIR}/ch_attribution.config --centersFile ${WORKDIR}/*.centers.txt ${input} --output ${WORKDIR} && \
/code/SPoCA/bin1/get_CH_map.x --config ${CONFIGDIR}/get_ch_map.config ${WORKDIR}/*.SegmentedMap.fits $input --output $output

rm -rf ${WORKDIR}
