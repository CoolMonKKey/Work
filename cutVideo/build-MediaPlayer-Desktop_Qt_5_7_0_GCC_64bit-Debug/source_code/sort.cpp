#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<algorithm>

using namespace std;

int main()
{
    ofstream ocout;
    fstream ftp("new_video/filelist.txt");
    if(!ftp)
    {
        cout<<"floder is wring!"<<endl;
        return 0;
    }
    vector<string> words;
    string line;

    while(getline(ftp,line))
    {
        words.push_back(line);
    }
    
    sort(words.begin(),words.end());

    for(int i=0;i<words.size();++i)
    {
        cout<<words[i]<<endl;
    }
    ocout.open("new_video/filelist.txt");
    for(int i=0;i<words.size();++i)
    {
        ocout<<words[i]<<endl;
    }
    ocout.close();
    return 0;
}