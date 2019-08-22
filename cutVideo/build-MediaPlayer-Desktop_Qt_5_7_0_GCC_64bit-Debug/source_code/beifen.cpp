#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <ctime>
#include <pthread.h>

//#include "time.h"

using namespace std;

pthread_mutex_t lock;
bool is_cut_whole = false;
//filelist.txt文件流
ofstream ocout;

string folder_path,
    cut_day,
    cut_start_time,
    cut_end_time,
    video_length,
    file_start_time,
    file_end_time;

int GetOperation(struct dirent *ptr);
void Copy(struct dirent *ptr);
void CutWhole(struct dirent *ptr, string command_ffmpeg);
void CutSmallHead(struct dirent *ptr, string command_ffmpeg);
void CutBigHead(struct dirent *ptr, string command_ffmpeg);
void AddParameter34(string command_ffmpeg, string new_start, string new_end);

bool IsH264File(string str);
string GetEndTime(string start_time);
string calc(string small_time, string big_time, string daytime, bool flag);

///////////////////////////////////////////////////////////////////////////////////

/*
    int     operation
    1       no need operation
    2       copy
    3       cut whole
    4       cut small head
    5       cut big head
*/

string GetEndTime(string start_time, string video_length)
{
    string end_time = start_time;
    string substr_minutes_time = start_time.substr(3, 2);

    int length = atoi(video_length.c_str());
    int minutes_time = atoi(substr_minutes_time.c_str()) + length;

    if (minutes_time > 60)
    {
        int hour_time = atoi(start_time.substr(0, 2).c_str()) + 1;
        string new_hour_time = std::to_string(hour_time);

        end_time = end_time.replace(0, 2, new_hour_time);
        end_time[3] = '0';
    }
    else
    {
        string minutes = std::to_string(minutes_time);
        end_time = end_time.replace(3, 2, minutes);
    }
    return end_time;
}

string calc(string small_time, string big_time, string daytime)
{
    time_t first, next;
    tm Tm;

    small_time = daytime + " " + small_time;
    big_time = daytime + " " + big_time;
    strptime(small_time.c_str(), "%Y-%m-%d %H:%M:%S", &Tm);
    first = mktime(&Tm);
    cout << asctime(&Tm);

    strptime(big_time.c_str(), "%Y-%m-%d %H:%M:%S", &Tm);
    next = mktime(&Tm);
    cout << asctime(&Tm);

    string time;
    double sec = difftime(next, first);
    int seconds = (int)sec;

#if 1
    int minutes = (seconds / 60);

    if (minutes == 0)
    {
        time = "00:00:";
        string miao = std::to_string(seconds);
        time = time + miao;

        return time;
    }
    else
    {
        time = "00:0";
        string Sec = std::to_string(minutes);
        time = time + Sec + ":";

        int miao = seconds % 60;

        if (miao < 10)
            time = time + "0";

        string Miao = std::to_string(miao);
        time += Miao;

        return time;
    }
#endif
}

bool IsH264File(string str)
{
    if (strlen(str.c_str()) <= 5)
        return false;

    const char *begin_filename_pos = str.c_str();
    const char *extension_fimename_file = begin_filename_pos + (strlen(begin_filename_pos)) - 5;
    string afterfour_pos = extension_fimename_file;

    if (*begin_filename_pos == '_')
        return false;

    if (afterfour_pos == ".h264")
        return true;

    return false;
}

/*
    int     operation
    1       no need operation
    2       copy
    3       cut whole
    4       cut small head
    5       cut big head
*/
int GetOperation()
{
    if (file_end_time <= cut_start_time || file_start_time >= cut_end_time)
        return 1;

    if (file_start_time >= cut_start_time && file_end_time <= cut_end_time)
        return 2;

    if (file_start_time <= cut_start_time && file_end_time >= cut_start_time && cut_end_time >= file_end_time)
        return 4;

    if (file_start_time <= cut_end_time && file_end_time >= cut_end_time && cut_start_time <= file_start_time)
        return 5;

    if (file_start_time <= cut_start_time && file_end_time >= cut_end_time)
        return 3;
}

void Copy(struct dirent *ptr)
{   
    cout << "copy this file!" << endl;
    string copy_command = "script/./cp_video.sh "; 
    copy_command = copy_command + ptr->d_name + " ";

//cout<<"1";
    char filename[50] = {0};
    string timeflag = "-";
    strcpy(filename,ptr->d_name);
    string videostartime = filename;
    videostartime = videostartime.substr(21,8);
    string videoendtime = GetEndTime(videostartime,video_length);
   // cout<<"videoendtime:"<<videoendtime<<endl;
   
    videostartime = videostartime.replace(2,1,timeflag);
    videostartime = videostartime.replace(5,1,timeflag);
 
    videoendtime = videoendtime.replace(2,1,timeflag);
    videoendtime = videoendtime.replace(5,1,timeflag);
    
    string copyfilename = videostartime + '-' + videoendtime;
    
    cout<<"copyfilename:"<<copyfilename<<endl;
    copy_command = copy_command + copyfilename; 

    copyfilename = "file '" + copyfilename +".mp4'\n";
    //copyfilename.erase(21,1);

    //ocout.open("new_video/filelist.txt");
    //pthread_mutex_lock(&lock); 
    ocout<<copyfilename.c_str();
    //pthread_mutex_unlock(&lock);
    //ocout.close();

    cout << "copy_command:" << copy_command.c_str() << endl;
    system(copy_command.c_str());
    
 }

void CutWhole(struct dirent *ptr, string command_ffmpeg)
{
    cout << "cut the whole file!" << endl;
    is_cut_whole = true;
    string new_start = calc(file_start_time, cut_start_time, cut_day);
    string new_end = calc(file_start_time, cut_end_time, cut_day);

    //parameter2->output_file_name

    string parameter2 = cut_start_time + "-" + cut_end_time + ".mp4 ";
    command_ffmpeg = command_ffmpeg + parameter2;

    cout<<"command_ffmpeg:"<<command_ffmpeg.c_str();

    //文件名存入filelist.txt
    //string videoname = parameter2;
    
     string timeflag = "-";
     parameter2 = "file '" + parameter2 + "'\n";
     parameter2.erase(27,1);
     parameter2.replace(8,1,timeflag);
     parameter2.replace(11,1,timeflag);
     parameter2.replace(17,1,timeflag);
     parameter2.replace(20,1,timeflag);

     //ocout.open("new_video/filelist.txt");
     //pthread_mutex_lock(&lock);
     ocout<<parameter2.c_str();
     //pthread_mutex_unlock(&lock);
     //ocout.close();
     
     
     AddParameter34(command_ffmpeg, new_start, new_end);
}

void CutSmallHead(struct dirent *ptr, string command_ffmpeg)
{
    cout << "cut thw small head!" << endl;
    string new_start = calc(file_start_time, cut_start_time, cut_day);
    string new_end = "00:00:00";
    int int_video_length = atoi(video_length.c_str());

    new_end = new_end.replace(3, 2, video_length);

    if (int_video_length < 10)
    {
        new_end = "00:00:00";
        new_end = new_end.replace(4, 1, video_length);
    }

    //parameter2->output_file_name
    string parameter2 = cut_start_time + "-" + file_end_time + ".mp4 ";
    string timeflag = "-";
    parameter2.replace(2,1,timeflag);
    parameter2.replace(5,1,timeflag);
    parameter2.replace(11,1,timeflag);
    parameter2.replace(14,1,timeflag);
    command_ffmpeg = command_ffmpeg + parameter2;

    //文件名存入filelist.txt
     parameter2 = "file '" + parameter2 + "'\n";
     parameter2.erase(27,1);

     //ocout.open("new_video/filelist.txt");
     //pthread_mutex_lock(&lock);
     ocout<<parameter2.c_str();
     //pthread_mutex_unlock(&lock);
     //ocout.close();

     AddParameter34(command_ffmpeg, new_start, new_end);

    // string parameter2 = cut_start_time + "-" + file_end_time + ".mp4 ";
    
    // command_ffmpeg = command_ffmpeg + parameter2;

    // //文件名存入filelist.txt
    //  parameter2 = "file '" + parameter2 + "'\n";
    //  parameter2.erase(27,1);
         
    //  string timeflag = "-";
    //  parameter2.replace(8,1,timeflag);
    //  parameter2.replace(11,1,timeflag);
    //  parameter2.replace(17,1,timeflag);
    //  parameter2.replace(20,1,timeflag);

    //  ocout<<parameter2.c_str();

    // AddParameter34(command_ffmpeg, new_start, new_end);
}

void CutBigHead(struct dirent *ptr, string command_ffmpeg)
{
    cout << "cut the big head!" << endl;
    string new_start = "00:00:00";
    string new_end = calc(file_start_time, cut_end_time, cut_day);
    cout << "new_end:" << new_end << endl;

    string parameter2 = file_start_time + "-" + cut_end_time + ".mp4 ";
         string timeflag = "-";
     parameter2.replace(2,1,timeflag);
     parameter2.replace(5,1,timeflag);
     parameter2.replace(11,1,timeflag);
     parameter2.replace(14,1,timeflag);
     command_ffmpeg = command_ffmpeg + parameter2;

         //文件名存入filelist.txt
     parameter2 = "file '" + parameter2 + "'\n";
     parameter2.erase(27,1);

     //ocout.open("new_video/filelist.txt");
     //pthread_mutex_lock(&lock);
     ocout<<parameter2.c_str();
     //pthread_mutex_unlock(&lock);
     //ocout.close();
     
     AddParameter34(command_ffmpeg, new_start, new_end);

    // //文件名存入filelist.txt
    //  parameter2 = "file '" + parameter2 + "'\n";
    //  parameter2.erase(27,1);
     
    //  string timeflag = "-";
    //  parameter2.replace(8,1,timeflag);
    //  parameter2.replace(11,1,timeflag);
    //  parameter2.replace(17,1,timeflag);
    //  parameter2.replace(20,1,timeflag);

    //  ocout<<parameter2.c_str();

    //  AddParameter34(command_ffmpeg, new_start, new_end);


}

void AddParameter34(string command_ffmpeg, string new_start, string new_end)
{
    //parameter3->folder path
    // string parameter3 = folder_path;
    // parameter3 = parameter3 + " ";
    // command_ffmpeg = command_ffmpeg + parameter3;

    //parameter3->start
    new_start = new_start + " ";
    command_ffmpeg = command_ffmpeg + new_start;

    //parameter4->end
    command_ffmpeg = command_ffmpeg + new_end;

    //cout<<"cut this file!"<<endl;
    cout << "command:" << command_ffmpeg << endl;
    system(command_ffmpeg.c_str());
}

void CutVideo(DIR *dir)
{
    struct dirent *ptr;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (is_cut_whole)
            break;

        if (ptr->d_type == DT_REG)
        {
            if (!IsH264File(ptr->d_name))
                continue;

            int file_length = strlen(ptr->d_name);
            char buf[file_length + 1] = {0};
            char bufname[file_length + 1] = {0};

            strcpy(bufname, ptr->d_name);
            strcpy(buf, ptr->d_name);

            string file_day_time = strtok(buf, ",");
            file_day_time = file_day_time.substr(10, 10);
            file_start_time = strtok(NULL, ".");
            file_end_time = GetEndTime(file_start_time, video_length);

            if (file_day_time != cut_day)
            {
                cout << "the input day != file_day,please Reenter!" << endl;
                continue;
            }

            cout << "cut_start_time->" << cut_start_time << " cut_end_time->" << cut_end_time << endl;
            cout << "file start_time->" << file_start_time << " file end_time->" << file_end_time << endl;

            /*
                int     operation
                1       no need operation
                2       copy
                3       cut whole
                4       cut small head
                5       cut big head
            */

            string command_ffmpeg = "script/./ffmpeg.sh ";
            string parameter1 = ptr->d_name;
            command_ffmpeg = command_ffmpeg + parameter1 + " ";

            int operation = GetOperation();

            //把要切割的文件名存到filelist.txt中
            // if (operation >= 2 && operation <= 5)
            // {
            //     string videoname = ptr->d_name;

            //     videoname += '\n';

            //     ocout<<videoname.c_str();
            // }

            switch (operation)
            {
            case 1:
                cout << "file: " << ptr->d_name << " is Beyond the range,Don't do angthing!" << endl;
                break;
            case 2:
                Copy(ptr);
                break;
            case 3:
                CutWhole(ptr, command_ffmpeg);
                break;
            case 4:
                CutSmallHead(ptr, command_ffmpeg);
                break;
            case 5:
                CutBigHead(ptr, command_ffmpeg);
                break;
            default:
                break;
            }
            cout << "--------------------------------------------------------------------------------------------------------" << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cout << "input is fail,please try again!" << endl;
        cout << "jusi like-->./cut_video /home/cidi/Documents/work/test_video/ 2019-03-04-16-35-59 2019-03-04-16-36-02 10" << endl;
        return -1;
    }

    //Collate input parameters
    folder_path = argv[1];
    string begin = argv[2];
    string end = argv[3];
    video_length = argv[4];

    cut_day = begin.substr(0, 10);

    //16-35-59-> 16:35:59 16-36-02->16:36:02
    cut_start_time = begin.substr(11, 8);
    cut_start_time = cut_start_time.replace(2, 1, ":").replace(5, 1, ":");
    cut_end_time = end.substr(11, 8).replace(2, 1, ":").replace(5, 1, ":");

    DIR *dir;
    if ((dir = opendir(folder_path.c_str())) == NULL)
    {
        cout << "this folder is failed:" << folder_path << endl;
        return -1;
    }

    if (argv[2] >= argv[3])
    {
        cout << "the input start_time >= end_time,please Reenter!" << endl;
        return -1;
    }

    ocout.open("new_video/filelist.txt");
    CutVideo(dir);
    ocout.close();

    return 0;
}
