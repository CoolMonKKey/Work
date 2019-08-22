#ifndef COMPTHREAD_H
#define COMPTHREAD_H

#include <QThread>
#include <thread>


class CompThread : public QThread
{
    Q_OBJECT

public:
    explicit CompThread(){};

public:
    virtual void run();

signals:
    void MsgSignal(const std::thread::id& tep);
};

#endif // COMPTHREAD_H
