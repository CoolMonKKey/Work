#!/bin/sh

echo "$1"   #input file name
echo "$2"   #output file name
#echo "$3"   #folder path
echo "$3"   #start_time
echo "$4"   #end_time or Length

basedir=`cd ../$(dirname $0); pwd -P`

#$3/$1 /home/cidi/Documents/work/test_csv/$1 输出路径
#      /home/cidi/Documents/work/new_video/$2 生成新文件路径

   ffmpeg -i $basedir/video_h264/$1 -vcodec copy -acodec copy -ss $3 -to $4 $basedir/new_video/$2 -y
