#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <ctime>
#include <pthread.h>

using namespace std;

pthread_mutex_t lock;
bool is_cut_whole = false;
//filelist.txt文件流
ofstream ocout;

int video_length = 0;

string folder_path,
    cut_day,
    cut_start_time,
    cut_end_time,
    file_start_time,
    file_end_time,
    video_folder_path;

int GetOperation();
void Copy(const struct dirent *ptr);
void CutWhole(const struct dirent *ptr, string command_ffmpeg);
void CutSmallHead(const struct dirent *ptr, string command_ffmpeg);
void CutBigHead(const struct dirent *ptr, string command_ffmpeg);
void AddParameter34(string command_ffmpeg, string new_start, string new_end);

bool IsH264File(string str);
int calcLenvideo();
string GetEndTime(string start_time);
string calc(const string small_time, const string big_time, const string daytime);
string GetEndTime_1(const string start_time, const int video_len);
string GetfileName(string temporary);
string Reparameter2(string parameter2);
//string GetfileName(string)

///////////////////////////////////////////////////////////////////////////////////

/*
    int     operation
    1       no need operation
    2       copy
    3       cut whole
    4       cut small head
    5       cut big head
*/

string Reparameter2(string parameter2)
{
    parameter2.replace(2, 1, "-");
    parameter2.replace(5, 1, "-");
    parameter2.replace(11, 1, "-");
    parameter2.replace(14, 1, "-");
    return parameter2;
}

string GetfileName(string temporary)
{
    temporary = "file '" + temporary + "'\n";
    temporary.replace(8, 1, "-");
    temporary.replace(11, 1, "-");
    temporary.replace(17, 1, "-");
    temporary.replace(20, 1, "-");
    return temporary;
}

int calcLenvideo()
{
    string file_name1, file_name2;
    struct dirent **entry_list;
    int len_video, count, i = 0;

    count = scandir(video_folder_path.c_str(), &entry_list, 0, alphasort);
    if (count < 4)
    {
        perror("scandir");
        return 0;
    }

    for (i = 2; i < 4; i++)
    {
        struct dirent *entry;

        entry = entry_list[i];
        if (i == 2)
            file_name1 = entry->d_name;
        if (i == 3)
            file_name2 = entry->d_name;
        free(entry);
    }
    free(entry_list);

    file_name1 = file_name1.substr(18, 6);
    file_name2 = file_name2.substr(18, 6);

    int h1 = std::atoi((file_name1.substr(0, 2).c_str()));
    int m1 = std::atoi((file_name1.substr(2, 2).c_str()));
    int s1 = std::atoi((file_name1.substr(4, 2).c_str()));

    int h2 = std::atoi((file_name2.substr(0, 2).c_str()));
    int m2 = std::atoi((file_name2.substr(2, 2).c_str()));
    int s2 = std::atoi((file_name2.substr(4, 2).c_str()));

    int timesec1 = h1 * 3600 + m1 * 60 + s1;
    int timesec2 = h2 * 3600 + m2 * 60 + s2;

    return timesec2 - 1 - timesec1;
}

string GetEndTime(string start_time, string video_length)
{
    // string end_time = start_time;
    // string substr_minutes_time = start_time.substr(3, 2);

    // int length = atoi(video_length.c_str());
    // int minutes_time = atoi(substr_minutes_time.c_str()) + length;

    // if (minutes_time > 60)
    // {
    //     int hour_time = atoi(start_time.substr(0,2).c_str()) + 1;
    //     string new_hour_time = std::to_string(hour_time);

    //     minutes_time = minutes_time%60;
    //     string new_minutes = std::to_string(minutes_time);
    //     end_time = end_time.replace(4,1,new_minutes.c_str());
    //     end_time = end_time.replace(0, 2, new_hour_time);
    //     end_time = end_time.replace(3,1,"0");
    // }
    // else
    // {
    //     string minutes = std::to_string(minutes_time);
    //     end_time = end_time.replace(3, 2, minutes);
    // }
    // return end_time;
}

string GetEndTime_1(const string start_time, const int video_len)
{
    int h, m, s = 0;
    string hour, min, sec;
    int h_time = std::stoi(start_time.substr(0, 2));
    int m_time = std::stoi(start_time.substr(2, 2));
    int s_time = std::stoi(start_time.substr(4, 2));

    cout << "h_time:" << h_time << endl;
    cout << "h_time*3600 + m_time*60 + s_time:" << h_time * 3600 + m_time * 60 + s_time << endl;

    cout << video_len << endl;
    int timestamp = h_time * 3600 + m_time * 60 + s_time + video_len;
    cout << "timestamp:" << timestamp << endl;

    h = timestamp / 3600;
    m = (timestamp - h * 3600) / 60;
    s = timestamp % 60;

    if (h < 10)
        hour = "0" + std::to_string(h) + ":";
    else
        hour = std::to_string(h) + ":";

    if (m < 10)
        min = "0" + std::to_string(m) + ":";
    else
        min = std::to_string(m) + ":";

    if (s < 10)
        sec = "0" + std::to_string(s);
    else
        sec = std::to_string(s);

    return hour + min + sec;
}

string calc(string small_time, string big_time, const string daytime)
{
    time_t first, next;
    tm Tm;

    // cout<<"small_time"<<small_time<<endl;
    // cout<<"big_time"<<big_time<<endl;

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

void Copy(const struct dirent *ptr)
{
    cout << "copy this file!" << endl;
    string copy_command = "script/./cp_video.sh ";
    copy_command = copy_command + ptr->d_name + " ";

    string timeflag = "-";
    string videostarttime = file_start_time.replace(2, 1, timeflag);
    videostarttime = videostarttime.replace(5, 1, timeflag);

    string videoendtime = file_end_time.replace(2, 1, timeflag);
    videoendtime = videoendtime.replace(5, 1, timeflag);

    string copyfilename = videostarttime + '-' + videoendtime;

    cout << "copyfilename:" << copyfilename << endl;
    copy_command = copy_command + copyfilename;

    copyfilename = "file '" + copyfilename + ".mpeg'\n";

    ocout << copyfilename.c_str();

    cout << "copy_command:" << copy_command.c_str() << endl;
    system(copy_command.c_str());
}

void CutWhole(const struct dirent *ptr, string command_ffmpeg)
{
    cout << "cut the whole file!" << endl;
    is_cut_whole = true;

    cout << "file_start_time" << file_start_time << endl;
    cout << "file_end_time" << file_end_time << endl;

    string new_start = calc(file_start_time, cut_start_time, cut_day);
    string new_end = calc(file_start_time, cut_end_time, cut_day);

    //parameter2->output_file_name

    string timeflag = "-";
    string parameter2 = cut_start_time + "-" + cut_end_time;
    string temporary = parameter2;

    parameter2 = Reparameter2(parameter2);
    command_ffmpeg = command_ffmpeg + parameter2 + " ";
    temporary = GetfileName(temporary);

    ocout << temporary.c_str();

    AddParameter34(command_ffmpeg, new_start, new_end);
}

void CutSmallHead(const struct dirent *ptr, string command_ffmpeg)
{
    cout << "cut thw small head!" << endl;
    string new_start = calc(file_start_time, cut_start_time, cut_day);
    string new_end = GetEndTime_1("000000", video_length);

    string timeflag = "-";
    string parameter2 = cut_start_time + "-" + file_end_time;
    string temporary = parameter2 + ".mpeg";

    parameter2 = Reparameter2(parameter2);
    command_ffmpeg = command_ffmpeg + parameter2 + " ";
    temporary = GetfileName(temporary);

    ocout << temporary.c_str();
    AddParameter34(command_ffmpeg, new_start, new_end);
}

void CutBigHead(const struct dirent *ptr, string command_ffmpeg)
{
    cout << "cut the big head!" << endl;
    string new_start = "00:00:00";
    string new_end = calc(file_start_time, cut_end_time, cut_day);
    cout << "new_end:" << new_end << endl;

    string timeflag = "-";
    string parameter2 = file_start_time + "-" + cut_end_time;
    string temporary = parameter2 + ".mpeg";

    parameter2 = Reparameter2(parameter2);
    command_ffmpeg = command_ffmpeg + parameter2 + " ";
    temporary = GetfileName(temporary);

    ocout << temporary.c_str();

    AddParameter34(command_ffmpeg, new_start, new_end);
}

void AddParameter34(string command_ffmpeg, string new_start, string new_end)
{
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
    const struct dirent *ptr;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (is_cut_whole)
            break;

        if (ptr->d_type == DT_REG)
        {
            // if (!IsH264File(ptr->d_name))
            //     continue;

            int file_length = strlen(ptr->d_name);
            char buf[file_length + 1] = {0};
            char bufname[file_length + 1] = {0};

            string file_day_time = ptr->d_name;
            file_day_time = file_day_time.substr(10, 8);
            file_day_time = file_day_time.insert(4, "-");
            file_day_time = file_day_time.insert(7, "-");

            file_start_time = ptr->d_name;
            file_start_time = file_start_time.substr(18, 6);
            file_end_time = GetEndTime_1(file_start_time, video_length);
            file_start_time = file_start_time.insert(2, ":");
            file_start_time = file_start_time.insert(5, ":");

            cout << "file_day_time:!" << file_day_time << endl;
            cout << "file_start_time:!" << file_start_time << endl;
            cout << "file_end_time:!" << file_end_time << endl;

            cout << file_day_time << endl;
            cout << cut_day << endl;
            if (file_day_time != cut_day)
            {
                cout << "the input day != file_day,please Reenter!" << endl;
                //cout<<"--------------------------------------------"<<endl;
                continue;
            }

            cout << "cut_start_time->" << cut_start_time << " cut_end_time->" << cut_end_time << endl;
            cout << "file start_time->" << file_start_time << " file end_time->" << file_end_time << endl;
#if 1

            /*
                int     operation
                1       no need operation
                2       copy
                3       cut whole
                4       cut small head
                5       cut big head
            */

            string command_ffmpeg = "script/./ffmpeg.sh ";
            string parameter1 = video_folder_path + ptr->d_name;
            command_ffmpeg = command_ffmpeg + parameter1 + " ";

            int operation = GetOperation();

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
#endif
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
    video_folder_path = argv[2];
    string begin = argv[3];
    string end = argv[4];

    cut_day = begin.substr(0, 10);

    //16-35-59-> 16:35:59 16-36-02->16:36:02
    cut_start_time = begin.substr(11, 8);
    cut_start_time = cut_start_time.replace(2, 1, ":").replace(5, 1, ":");
    cut_end_time = end.substr(11, 8).replace(2, 1, ":").replace(5, 1, ":");

    DIR *dir;
    if ((dir = opendir(video_folder_path.c_str())) == NULL)
    {
        cout << "this folder is failed:" << video_folder_path << endl;
        return -1;
    }

    video_length = calcLenvideo();
    if (video_length == 0)
        return -1;

    cout << "video_length:" << video_length << endl;

    ocout.open("new_video/filelist.txt");
    if (!ocout.is_open())
    {
        cout << "open filelist.txt is falure!" << endl;
        ocout.close();
        return -1;
    }
    CutVideo(dir);
    ocout.close();

    return 0;
}
