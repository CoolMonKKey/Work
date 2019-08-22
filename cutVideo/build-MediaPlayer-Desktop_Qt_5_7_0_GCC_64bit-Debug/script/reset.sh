#!/bin/sh

DIR=`cd ../$(dirname $0); pwd -P`

echo $DIR

cd $DIR
rm -rf new_video
mkdir new_video