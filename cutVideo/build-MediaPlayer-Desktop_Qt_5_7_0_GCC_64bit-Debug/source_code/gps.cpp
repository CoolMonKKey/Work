#include<iostream>
#include<dirent.h>
#include <fstream>
#include<string.h>
#include<math.h>

using namespace std;

#define PI                      3.1415926
#define EARTH_RADIUS            6378.137        //地球近似半径

 double radian( double d);
 double get_distance( double lat1,  double lng1,  double lat2,  double lng2);

// 求弧度
 double radian( double d)
{
    return d * PI / 180.0;   //角度1˚ = π / 180
}

//计算距离
 double get_distance( double lat1,  double lng1,  double lat2,  double lng2)
{
     double radLat1 = radian(lat1);
     double radLat2 = radian(lat2);
     double a = radLat1 - radLat2;
     double b = radian(lng1) - radian(lng2);
    
     double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));
    
    dst = dst * EARTH_RADIUS;
    //dst= round(dst * 10000) / 10; 四舍五入
    dst = (dst*10000) / 10;  
    return dst;
}

int main(int argc,char **argv)
{
    DIR *dir;
    if((dir=opendir(argv[1]))==nullptr)
    {
        cout<<"open the dir error!"<<endl;
        return -1;
    }

    struct dirent *ptr;
    
    while((ptr=readdir(dir))!=nullptr)
    {

        if(ptr->d_type != DT_REG)
            continue;
        
        string file_name = ptr->d_name;
        string path = "gps_csv/" + file_name;
        ifstream inFile(path.c_str(),ios::in);
        string lineStr;
        if(!inFile.good())
        {
            cout<<"ERROW:inFile the "<<ptr->d_name<<" read file error"<<endl;
            continue;
        }
      
        string extren_name = "_CIDI_";
        string filename = ptr->d_name;
        string name = extren_name + file_name;

        ofstream outFile;
        outFile.open(name.c_str(),ios::out);
        if(!outFile.good())
        {
            cout<<"ERROE:outFile the"<<ptr->d_name<<"write file error!"<<endl;
            continue;
        }

        int length = 0;
        bool first_row = true;
        bool second_row = true;
        string last_time, 
               last_second_speed,
               last_second_horspd,
               last_second_vertspd,
               last_latitude,
               last_longitude;

        while(getline(inFile,lineStr))
        {
            if(first_row)
            {
                string title = ",velocity,acceleration,hor_spd_acceleration,vert_spd_acceleration,transverse_acceleration";

                first_row = false;
                length = lineStr.length() + 1;
                lineStr = lineStr + title;

                outFile<<lineStr<<endl;
                continue;
            }
 
            char buf[length] = {0};
            strcpy(buf,lineStr.c_str());
            string current_time = strtok(buf,",");
            string hor_spd = strtok(NULL,",");
            string vert_spd = strtok(NULL,",");
            string latitude = strtok(NULL,",");
            string longitude = strtok(NULL,",");

            double instance = hypot(atof(hor_spd.c_str()),atof(vert_spd.c_str()));
            string current_speed = std::to_string(instance);
            if(*vert_spd.begin() == '-')
                current_speed = '-' + current_speed;

            string a,
                   horspd_a,
                   vertspd_a,
                   transverse_a;
            if(second_row)
            {
                second_row = false;
                last_time = current_time;
                last_second_speed = current_speed;
                last_second_horspd = hor_spd;
                last_second_vertspd = vert_spd;
                last_latitude = latitude;
                last_longitude = longitude;

                lineStr = lineStr + ',' + current_speed + ",0" + ",0" + ",0" + ",0";

                outFile<<lineStr<<endl;
                continue;
            }

            a = std::to_string(atof(current_speed.c_str()) - atof(last_second_speed.c_str()));
            horspd_a = std::to_string(atof(hor_spd.c_str()) - atof(last_second_horspd.c_str()));
            vertspd_a = std::to_string(atof(vert_spd.c_str()) - atof(last_second_vertspd.c_str()));

            //calc transverse_acceleration
            double real_distance = get_distance(atof(last_latitude.c_str()),
                                           atof(last_longitude.c_str()),
                                           atof(latitude.c_str()),
                                           atof(longitude.c_str()));
            //以下的数据全是两个点之间的
            
            //时间差                               
            double differ_time = (atof(current_time.c_str()) - atof(last_time.c_str()));
            //水平平均速度    
            double hor_dis_v = (atof(hor_spd.c_str()) + atof(last_second_horspd.c_str())) / 2;
            //水平距离
            double hor_dis = hor_dis_v * (differ_time/1000);
            //前一刻的横向速度
            double last_sec_trans_spd = (tan(acos(hor_dis/real_distance))) * stof(last_second_horspd.c_str());
            //后一刻的横向速度
            double this_sec_tran_spd = (tan(acos(hor_dis/real_distance))) * stof(hor_spd.c_str());
            //横向加速度 m/s2
            transverse_a = std::to_string((this_sec_tran_spd - last_sec_trans_spd) * (1000/differ_time));
#if 0
 
            cout<<"real_distance:"<<real_distance<<endl;
            cout<<"hor_dis:"<<hor_dis<<endl;
            cout<<"last_second_horspd:"<<last_second_horspd<<endl;
            cout<<"last_second_horspd:"<<stof(last_second_horspd.c_str())<<endl;
            cout<<"hor_spd:"<<hor_spd<<endl;
            cout<<"hor_spd:"<<stof(hor_spd.c_str())<<endl;
            cout<<"last_sec_trans_spd:"<<last_sec_trans_spd<<endl;
            cout<<"this_sec_tran_spd:"<<this_sec_tran_spd<<endl;
            cout<<"acos:"<<acos(hor_dis/real_distance)<<endl;
            cout<<"tan:"<<tan(acos(hor_dis/real_distance))<<endl;
            cout<<"last_second_horspd*(tan(acos(hor_dis/real_distance)))"<<(tan(acos(hor_dis/real_distance)))*stof(last_second_horspd.c_str())<<endl;
            break;            

#endif
            if(hor_dis >= real_distance)
                transverse_a = '0';

            lineStr = lineStr + ',' + current_speed + ',' + a + ',' + horspd_a + ',' + vertspd_a + ',' + transverse_a;
            outFile<<lineStr<<endl;

            last_second_speed = current_speed;
            last_second_horspd = hor_spd;
            last_second_vertspd = vert_spd; 
            last_latitude = latitude;
            last_longitude = longitude;
            last_time = current_time;  

        }

        string command = "script/./cp_gps_csv.sh ";
        command += name;
        system(command.c_str());

        cout<<"The file_name :"<<ptr->d_name<<"is finished"<<endl; 
        cout<<"---------------------------------------------------------------"<<endl; 

    }

    return 0;
}