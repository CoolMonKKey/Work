#!/bin/sh

#echo "$1"
#echo "$2"
#echo "$3"
#echo "$4"

DIR=`cd ../$(dirname $0); pwd -P`

echo "***********merge the shipin file!**************"
echo $1
ffmpeg -f concat -i $DIR/new_video/filelist.txt -c copy new_video/rtsp-server-0.mp4

#ffmpeg -i new_video/rtsp-server-0.mp4 -b:v 6000K new_video/output.mp4
