#!/bin/sh

path=$1

if [ ! -d ${path} ]; then
    # Directory does not exists.    
    mkdir -p ${path}
    # echo "Make Directory \"${path}\"."
    
# else
    # Directory exists.
    # echo "Directory \"${path}\" already exists."
fi