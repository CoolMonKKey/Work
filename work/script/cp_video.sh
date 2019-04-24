#!/bin/sh

echo "$1" #需要拷贝的视频文件名

basedir=`cd ../$(dirname $0); pwd -P`

out_path=$basedir/video_h264/$1
in_path=$basedir/new_video/

cp -r $out_path $in_path


