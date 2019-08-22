#!/bin/sh

echo "$1" #需要拷贝的视频文件名
echo "$2" #拷贝后新的视频文件名

DIR=`cd ../$(dirname $0); pwd -P`

name="hello"
out_path=$DIR/video_h264/$1
in_path=$DIR/new_video/

cp -r $out_path $in_path$2$name.mpeg #拷贝文件到新的目录下

ffmpeg -i $in_path$2$name.mpeg -vcodec copy -f mp4 $in_path$2.mpeg

rm -rf $in_path$2$name.mpeg
 
#ffmpeg -i $in_path/$1 -vcodec copy -f mp4 $in_path/$2.mp4 #把拷贝过来的h264文件转换成MP4文件

#rm -rf $in_path/$1 #删除拷贝过来的h264文件


