#!/bin/sh

echo "$1"   #folder path /home/cidi/Documents/work/
echo "$2"   #start time 2019-03-04-16-20-00
echo "$3"   #end time   2019-03-04-16-30-00

echo "***********operating gps csv!******************"
./source_code/gps $1"gps_csv"

echo "***********operating topic csv!****************"
./source_code/cut_csv $1"topic_csv" $2 $3

echo "***********operating video_h264!***************"
./source_code/cut_video $1"video_h264" $2 $3 $4


 


