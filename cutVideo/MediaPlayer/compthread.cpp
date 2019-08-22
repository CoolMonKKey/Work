#include "compthread.h"

#include <QDebug>
#include <iostream>
#include <QString>

using namespace std;

void CompThread::run()
{

    QString compressionCommand = "./script/compression.sh";
    system(compressionCommand.toStdString().c_str());
}
