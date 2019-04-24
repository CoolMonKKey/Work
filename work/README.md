###切割csv文件h264文件工具

#文件
        start.sh 启动脚本
        topic_csv gps_csv video_h264 存放要处理前的csv、h264文件
        new_topic new_gps new_video 存放处理后的文件
        source_code 存放源码
        script 脚本目录 

#使用方法
        第一步:根据上文，把需要处理的文件按分类放入对应文件夹
        第二步:在当前目录下打开命令窗口，输入命令:
        cidi@cidi-OptiPlex-7050:~/Documents/work$ ./start.sh /home/cidi/Documents/work/ 2019-03-04-16-35-59 2019-03-04-16-36-02 10
       （10:视频的长度是10分钟，这里的目录是我本地地址、拷贝过去需要改成你的地址）

#注意事项
        1、存放的视频文件长度必须一致！
        2、.h264的文件名文件格式必须是:video_all_2019-03-27,09:14:17.h264一类的！


        
