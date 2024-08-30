#!/bin/bash
set -e
input=$1
input=`realpath $input`

if [ -z $2 ]; then
    output="${input%.*}_seg.fits"
    outputdir=`dirname $output`
    outputfile=`basename $output`
else
    if [ -d $2 ]; then
        outputdir=$2
        tmp="${input%.*}_seg.fits"
        outputfile=`basename $tmp`
	output="${outputdir}/${outputfile}"
    else
        output=$2
        outputdir=`dirname $output`
        outputfile=`basename $output`
    fi
fi
output=`realpath $output`

WORKDIR=`mktemp -d`
scriptdir="$( dirname -- "$BASH_SOURCE"; )";
scriptdir=`realpath $scriptdir`
CONFIGDIR=$scriptdir/../configs

cd $WORKDIR
/code/SPoCA/bin1/classification.x --config ${CONFIGDIR}/ch_classification.config ${input} --output ${WORKDIR} && \
/code/SPoCA/bin1/attribution.x --config ${CONFIGDIR}/ch_attribution.config --centersFile ${WORKDIR}/*.centers.txt ${input} --output ${WORKDIR} && \
/code/SPoCA/bin1/get_CH_map.x --config ${CONFIGDIR}/get_ch_map.config ${WORKDIR}/*.SegmentedMap.fits $input --output ${WORKDIR}/$outputfile

cp ${WORKDIR}/$outputfile $output

cd -
rm -rf ${WORKDIR}
