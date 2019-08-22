#!/bin/sh

echo "input paramete just like:"
echo "./start.sh /home/cidi/Documents/work/ 2019-03-04-16-35-59 2019-03-04-16-36-02 10"

echo "$1"   #programe path /home/cidi/Documents/work/
echo "$2"   #videoFolder path
echo "$3"   #start time 2019-03-04-16-20-00
echo "$4"   #end time   2019-03-04-16-30-00
echo "$5"   #video length   05

echo "***********operating gps csv!******************"
#./source_code/gps $1"gps_csv"

echo "***********operating topic csv!****************"
#./source_code/cut_csv $1"topic_csv" $2 $3

echo "***********operating video_h264!***************"
 ./source_code/cut_video $1 $2 $3 $4 $5

echo "***********sort the shipin file!***************"
./source_code/sort

#echo "***********merge the shipin file!**************"
ffmpeg -f concat -i new_video/filelist.txt -c copy new_video/rtsp-server-0.mp4

#echo "***********compression the shipin file!**************"
#ffmpeg -i new_video/rtsp-server-0.mp4 -b:v 6000K new_video/output.mp4
#正在切割视屏，请稍等... 移动进度条
