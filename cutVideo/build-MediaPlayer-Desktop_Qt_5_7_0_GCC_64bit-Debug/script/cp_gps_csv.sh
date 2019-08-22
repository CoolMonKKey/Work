#!/bin/sh

#$1 新生成的文件 移动到新的文件夹下

DIR=`cd ../$(dirname $0); pwd -P`
mv  $1 $DIR/new_gps/