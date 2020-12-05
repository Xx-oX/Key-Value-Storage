#!/bin/sh

dir=$1
path=$2

if [ ! -d ${dir} ]; then
    mkdir -p ${dir}
fi
if [ ! -f ${path} ]; then
    # File does not exists.
    touch ${path}
    # echo "Make File \"${path}\"."
    
# else
    # File exists.
    # echo "File \"${path}\" already exists."    
fi