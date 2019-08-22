#include <iostream>
#include <time.h>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <sys/types.h>
#include <dirent.h> 
#include <unistd.h>
 
using namespace std;

/*  判断文件是否是csv文件
    输入:文件名
    输出:true or false
*/
bool IsCsvFile(string str);

/*  时间抓换成时间戳
    输入:argv[]
    输出:整形的时间戳
*/
int standard_to_stamp(char *str_time);

/*  获取timestamp_sec这个字符在csv表中的位置
    输入:csv表的第一行数据
    输出：int
*/
int GetTimestampSecPos(string lineStr);

/*  获取文件中所有csv文件的的文件名存在数组中，然后调用切割函数
    输入:文件夹的目录句柄、开始切割的时间戳、结束切割的时间戳
    输出:void
*/
void GetCsvList(DIR *dir,int s_stamptime,int e_stamptime);

////////////////////////////////////////////////////////////////////////////////

bool IsCsvFile(string str)
{
    const char *begin_filename_pos = str.c_str();
    const char *extension_fimename_file = begin_filename_pos+(strlen(begin_filename_pos))-4;
    string afterfour_pos = extension_fimename_file;

    if(afterfour_pos == ".csv")
            return true;

    
    return false;   
}

int standard_to_stamp(char *str_time)  
{  
	struct tm stm;  
	int iY, iM, iD, iH, iMin, iS;  
 
	memset(&stm,0,sizeof(stm));
   
    int count = 1;
    char c[] = "-";

    char *r = strtok(str_time,c);
    
    while(str_time != NULL)
    {
        if(count == 1)
        {
            iY = atoi(r);
            ++count;
            r = strtok(NULL,c);
            continue;
        }
        if(count == 2)
        {
            iM = atoi(r);
            ++count;
            r = strtok(NULL,c);
            continue;
        }
        if(count == 3)
        {
            iD = atoi(r);
            ++count;
            r = strtok(NULL,c);
            continue;
        }
        if(count == 4)
        {
            iH = atoi(r);
            ++count;
            r = strtok(NULL,c);
            continue;
        }
        if(count == 5)
        {
            iMin = atoi(r);
            ++count;
            r = strtok(NULL,c);
            continue;
        }
        if(count == 6)
        {
            iS = atoi(r);
            ++count;
            r = strtok(NULL,c);
            break;
        }       
    }

	stm.tm_year=iY-1900;  
	stm.tm_mon=iM-1;  
	stm.tm_mday=iD;  
	stm.tm_hour=iH;  
	stm.tm_min=iMin;  
	stm.tm_sec=iS;  
 
    return (int)mktime(&stm);  
}

int GetTimestampPos(string lineStr)
{
    int count=0;
    int len = strlen(lineStr.c_str());
    char buf[len+1]={0};

    strcpy(buf,lineStr.c_str());
    string cut_string = strtok(buf,",");

    while(1)
    {
        ++count;
        if(cut_string == "timestamp_sec")
            break;
        cut_string = strtok(NULL,",");
    }

    return count;
}

void GetCsvList(DIR *dir,int s_stamptime,int e_stamptime)
{
    struct dirent *ptr;
    int i=0,count=0;

    while((ptr = readdir(dir)) != NULL) 
    {
        if(ptr->d_type != DT_REG)
            continue;

        if(!IsCsvFile(ptr->d_name))
            continue;

        cout<<"The file_name:"<<ptr->d_name<<" "<<"is conversing"<<endl;
        string path = ptr->d_name;
        path = "topic_csv/" + path;
        ifstream inFile(path.c_str(),ios::in);
        string lineStr;
        if(!inFile.good())
        {
            cout<<"ERROW:inFile the "<<ptr->d_name<<" read file error"<<endl;
            cout<<"------------------------------------------------------"<<endl;
            continue;
        }

        string name = "_CIDI_";
        string new_file_name = name + ptr->d_name;
        
        ofstream outFile;
        outFile.open(new_file_name.c_str(),ios::out);
        if(!outFile.good())
        {
            cout<<"ERROE:outFile the"<<ptr->d_name<<"write file error!"<<endl;
            continue;
        }

        int timestamp_pos = 0;
        bool first_row = true;
        int length = 0;
        
        while(getline(inFile,lineStr))
        {
            int timestamp_offset;
             
            if(first_row)
            {
                first_row = false;
                //timestamp_pos = GetTimestampPos(lineStr);
                //cout<<"timestamp_pos:"<<timestamp_pos<<endl;

                timestamp_offset = lineStr.find("timestamp_sec",0) + 14;
            }

            string timestamp;
            timestamp = lineStr.substr(timestamp_offset,10);
            int time = atoi(timestamp.c_str());
            if(time >= s_stamptime && time <= e_stamptime)
                outFile<<lineStr<<endl;
            
#if 0
            if(first_row)
            {
                first_row = false;
                timestamp_pos = GetTimestampPos(lineStr);
                cout<<"timestamp_pos:"<<timestamp_pos<<endl;
            }

            length = lineStr.length();
            char buf[length+1] = {0};
            char cut_buf[50] = {0};
            strcpy(buf,lineStr.c_str());
            string str = strtok(buf,",");

            for(int i=0;i<timestamp_pos;++i)
                str = strtok(NULL,",");

            strcpy(cut_buf,str.c_str());

            int timestamp = atoi(cut_buf);
            if(timestamp >= s_stamptime && timestamp <= e_stamptime)
                outFile<<lineStr<<endl;
#endif
        }

        string command = "script/./cp_topic_csv.sh ";
        command += new_file_name;
        system(command.c_str());

        cout<<"The file_name :"<<ptr->d_name<<"is finished"<<endl; 
        cout<<"---------------------------------------------------------------"<<endl;              
    }

    closedir(dir);   
}

int main(int argc,char** argv)
{
    if(argc != 4)
    {
        cout<<"input is fail,please try again!"<<endl; 
        cout<<"jusi like-->./water /home/cidi/Documents/3-28/ 2019-03-04-16-35-59 2019-03-04-16-36-02"<<endl;
        return -1;
    }

    //处理csv文件
    DIR *dir; 
    if((dir = opendir(argv[1])) == NULL)
    {
        cout<<"open the dir error!"<<endl;
        return -1;
    }

    int start_stampTime = standard_to_stamp(argv[2]);
    int end_stampTime = standard_to_stamp(argv[3]);

    cout <<"开始的时间戳:"<<start_stampTime << endl;
    cout <<"结束的时间戳:"<<end_stampTime << endl;

    GetCsvList(dir,start_stampTime,end_stampTime);

    return 0;
}

      