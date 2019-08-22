#!/bin/sh

echo "$1"   #input file name
echo "$2"   #output file name
echo "$3"   #start_time
echo "$4"   #end_time or Length

DIR=`cd ../$(dirname $0); pwd -P`

echo 1
 #ffmpeg -i $DIR/video_h264/$1 -vcodec copy -acodec copy -ss $3 -to $4 $DIR/new_video/$2.mp4 -y

 #打开用户自定义的视频文件目录
 ffmpeg -i $1 -vcodec copy -acodec copy -ss $3 -to $4 $DIR/new_video/$2.mp4 -y
echo 2
 ffmpeg -i  $DIR/new_video/$2.mp4 -vcodec copy -f mp4 $DIR/new_video/$2.mpeg
echo 3
rm -rf $DIR/new_video/$2.mp4




   #$2 command:script/./ffmpeg.sh video_all_2019-03-04,16:34:19.h264 16:35:02-16:44:19.mp4 00:00:43 00:10:00

