#!/bin/sh

DIR=`cd ../$(dirname $0); pwd -P`

echo "PID of this script: $$"

echo "***********comperssion the shipin file!**************"

ffmpeg -i $DIR/new_video/rtsp-server-0.mp4 -b:v 6000K new_video/output.mp4
