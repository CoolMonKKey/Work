#!/bin/sh

#$1切割后的csv文件

basedir=`cd ../$(dirname $0); pwd -P`

out_path=$basedir/$1
in_path=$basedir/new_topic/

mv $out_path $in_path

echo "mv $1 success!"