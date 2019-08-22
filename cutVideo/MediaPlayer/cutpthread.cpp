#include "cutpthread.h"

#include <QString>
#include <QDebug>

using namespace std;

void Cutpthread::run()
{
    MediaPlayer Me;
    qDebug()<<Me.command;
    system(Me.command.toStdString().c_str());

}
