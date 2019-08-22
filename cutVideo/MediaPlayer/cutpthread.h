#ifndef CUTPTHREAD_H
#define CUTPTHREAD_H

#include "mediaplayer.h"
#include <QThread>
#include <thread>


class Cutpthread : public QThread
{
    Q_OBJECT

public:
    friend MediaPlayer;
public:
    explicit Cutpthread(){};

public:
    virtual void run();

public:
    //static QString MediaPlayer command;
};

#endif // CUTPTHREAD_H
