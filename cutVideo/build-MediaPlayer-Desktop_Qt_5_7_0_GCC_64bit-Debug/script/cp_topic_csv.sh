#!/bin/sh

#$1切割后的csv文件

DIR=`cd ../$(dirname $0); pwd -P`

out_path=$DIR/$1
in_path=$DIR/new_topic/

mv $out_path $in_path

echo "mv $1 success!"