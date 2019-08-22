#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMainWindow>
#include "customslider.h"
#include <thread>
#include <QString>
//#include "Cutpthread.h"

namespace Ui {
class MediaPlayer;
}

class MediaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    //friend class Cutpthread;
    explicit MediaPlayer(QWidget *parent = 0);
    ~MediaPlayer();

public:
    static QString command;
    QString spetime(QString time);
    QString settime(qint64 playtime);
    void setHeadTime(qint64 playtime);
    int get_timespace(QString time);
    //void setEndTime(qint64 playtime);

private slots:
    void on_pushButton_Volume_clicked();

    void on_pushButton_Open_clicked();

    void on_pushButton_Player_clicked();


    //自定义槽函数
    void horizontalSlider_clicked();

    void horizontalSlider_moved();

    void horizontalSlider_released();

    void slider_Volume_Changed();

    void onTimerOut();

    void on_toolButton_clicked();

    void on_toolButton_work_clicked();

    void on_toolButton__video_clicked();

    void on_pushButton_clicked();

    //video length
    void setEndTime(qint64 playtime);

    //video head time

    void on_radioButton_clicked(bool checked);

    void on_checkBox_clicked();

    void on_pushButton_video_clicked();

    void on_pushButton_reset_clicked();

    void on_checkBox_playtime_clicked();

    void on_checkBox_clicked(bool checked);

    void OnMsgSignal(const std::thread::id& tep);

private:
    Ui::MediaPlayer *ui;
    CustomSlider *slider_Volume;
};

#endif // MEDIAPLAYER_H
