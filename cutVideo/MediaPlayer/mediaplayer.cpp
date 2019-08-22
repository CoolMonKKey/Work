#include "mediaplayer.h"
#include "ui_mediaplayer.h"
#include "compthread.h"
#include "cutpthread.h"

#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QFileDialog>
#include <QProgressDialog>
#include <QFile>
#include <QMessageBox>
#include <QTimer>
#include <QStringList>
#include <QDir>
#include <string>
#include <iostream>
#include <QProgressBar>

using namespace std;

//播放视频的全局变量
QVBoxLayout* layout_video;//布局
QMediaPlayer* player;   //播放器
QVideoWidget* widget;   //视频播放控件
QTimer *timer;          //播放计时器
int maxValue = 1000;    //进度条最大值

//播放状态，true为播放，false为暂停
bool play_state;
//是否重新载入视频
bool if_reload=false;
//音量控制条是否出现
bool state_slider_volume = false;


MediaPlayer::MediaPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MediaPlayer)
{
    ui->setupUi(this);

    ui->dateTimeEdit_start->setDisplayFormat("yyyy-MM-dd-hh-mm-ss");
    ui->dateTimeEdit_end->setDisplayFormat("yyyy-MM-dd-hh-mm-ss");
    //音量调节：设置slider_volume,初始化、方向
    slider_Volume = new CustomSlider(this);
    slider_Volume->setOrientation(Qt::Vertical);

    //初始化不可用按钮
    ui->pushButton_Player->setEnabled(false);
    //ui->pushButton_Volume->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    slider_Volume->setEnabled(false);
    slider_Volume->hide();

    //连接信号与槽
    connect(ui->horizontalSlider,&CustomSlider::costomSliderClicked,
            this,&MediaPlayer::horizontalSlider_clicked);
    connect(ui->horizontalSlider,&CustomSlider::sliderMoved,
            this,&MediaPlayer::horizontalSlider_moved);
    connect(ui->horizontalSlider,&CustomSlider::sliderReleased,
            this,&MediaPlayer::horizontalSlider_released);
    connect(slider_Volume,&CustomSlider::costomSliderClicked,
            this,&MediaPlayer::slider_Volume_Changed);
    connect(slider_Volume,&CustomSlider::sliderMoved,
            this,&MediaPlayer::slider_Volume_Changed);
}

MediaPlayer::~MediaPlayer()
{
    delete ui;
}

QString MediaPlayer::command="hello world";

void MediaPlayer::on_pushButton_Open_clicked()
{
    //选择视频文件
    QString filename = QFileDialog::getOpenFileName(this,tr("选择视频文件"),"/home/cidi/Documents/gxs/Qt-MediaPlayer/build-MediaPlayer-Desktop_Qt_5_7_0_GCC_64bit-Debug/new_video/",
                                                    tr("视频格式(*.avi *.mp4 *.flv *.mkv *.mpeg)"));
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(NULL, "Title", "Content", QMessageBox::Ok,
                                 QMessageBox::Ok);
        return;
    }

    if(if_reload)
    {//重新加载视频时，重置以下变量
        delete layout_video;
        delete player;
        delete widget;
        delete timer;
    }
    if_reload = true;

    //实例化三个全局变量
    layout_video = new QVBoxLayout;
    player = new QMediaPlayer;
    widget = new QVideoWidget;
    //设置视频播放区域与Label的边距
    layout_video->setMargin(1);
    //根据label_player尺寸设置播放区域
    widget->resize(ui->label_play_video->size());
    layout_video->addWidget(widget);
    ui->label_play_video->setLayout(layout_video);
    player->setVideoOutput(widget);

    //设置播放器
    player->setMedia(QUrl::fromLocalFile(filename));
    //play_state为true表示播放，false表示暂停
    play_state = true;
    //ui->pushButton_Volume->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    ui->horizontalSlider->setRange(0,maxValue);
    //启用播放/暂停按钮，并将其文本设置为“暂停”
    ui->pushButton_Player->setEnabled(true);
    ui->pushButton_Player->setText("暂停");

    timer = new QTimer();
    timer->setInterval(1000);
    timer->start();

    connect(timer,SIGNAL(timeout()),this,SLOT(onTimerOut()));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(setEndTime(qint64)));
    //connect(timer,SIGNAL(timeout()),this,SLOT(setHeadTime()));

    //settime(qint64 playtime);
    //播放器开启
    player->play();

    //qDebug()<<"duration"<<player->duration();
}

void MediaPlayer::on_pushButton_Player_clicked()
{
    if(play_state){
        player->pause();
        ui->pushButton_Player->setText("播放");
    }
    else{
        player->play();
        ui->pushButton_Player->setText("暂停");
    }
    play_state = !play_state;
}

void MediaPlayer::slider_Volume_Changed()
{
    player->setVolume(slider_Volume->value());
}

void MediaPlayer::onTimerOut()
{
    ui->horizontalSlider->setValue(player->position()*maxValue/player->duration());
    int currentTime = ui->horizontalSlider->value()*player->duration()/maxValue;
    setHeadTime(currentTime);
}

void MediaPlayer::horizontalSlider_clicked()
{
    int currentTime = ui->horizontalSlider->value()*player->duration()/maxValue;
    player->setPosition(currentTime);
    qDebug()<<"currentTime:"<<currentTime;
}

void MediaPlayer::horizontalSlider_moved()
{
    timer->stop();
    player->setPosition(ui->horizontalSlider->value()*player->duration()/maxValue);
}

void MediaPlayer::horizontalSlider_released()
{
    timer->start();
}


void MediaPlayer::on_toolButton_work_clicked()
{

}

void MediaPlayer::on_toolButton__video_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("输入视频路径为空，请重新选择！"));
        return;
    }
    ui->lineEdit_video->setText(dir);


    QDir videodir(dir);
    QStringList nameFilters;
    QStringList files = videodir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    nameFilters << "*.mpeg";


    if(files.size() == 0)
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("请选择视频格式是.mpeg的视频文件夹！"));
        return;
    }

    if(files.size() == 1)
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("待切割的视频文件数需要大于两个，请重新选择视频文件夹！"));
        return;
    }

    QString s_video = files[0]; //20190624104844 video_all_20190624104844
    QString second_video = files[1];
    QString e_video = files[files.size()-1];

    s_video = QString::fromStdString(s_video.toStdString().substr(10,14));
    second_video  = QString::fromStdString(second_video.toStdString().substr(10,14));
    e_video = QString::fromStdString(e_video.toStdString().substr(10,14));

    int s_video_space = get_timespace(s_video);
    int sec_video_space = get_timespace(second_video);
    int len = sec_video_space- 1 - s_video_space;
    //qDebug()<<len;

    QString e_video_front = QString::fromStdString(e_video.toStdString().substr(10,8));

    qDebug()<<e_video;
    int e_video_space = get_timespace(e_video);

    int timespace = (e_video_space + len -1)*1000;
    QString fileEnd = QString::fromStdString(settime(timespace).toStdString().replace(2,1,"-"));
    fileEnd = QString::fromStdString(fileEnd.toStdString().replace(5,1,"-"));

    qDebug()<<fileEnd;

    QString wholeStartTime = spetime(s_video);
    QString wholeEndTime = QString::fromStdString(spetime(e_video).toStdString().substr(0,11));
    wholeEndTime += fileEnd;
    qDebug()<<wholeStartTime;
    qDebug()<<wholeEndTime;

    ui->label_svideo->setText(wholeStartTime);
    ui->label_endvideo->setText(wholeEndTime);

}


void MediaPlayer::on_pushButton_clicked()
{

}

void MediaPlayer::on_checkBox_clicked()
{

}

void MediaPlayer::on_radioButton_clicked(bool checked)
{

}

void MediaPlayer::setEndTime(qint64 playtime)
{
    QString str;
    playtime = player->duration();
    qDebug()<<playtime;
    str = settime(playtime);
    ui->label_video_end->setText(str);
}

void MediaPlayer::setHeadTime(qint64 playtime)
{
    QString str;
    str = settime(playtime);
    ui->label_video_start->setText(str);
}

QString MediaPlayer::settime(qint64 playtime)
{
    if(playtime <=0)
        return 0;

    int h,m,s;
    QString hour,min,sec;
    playtime += 1000;  //+1s
    playtime /= 1000;  //获得的时间是以毫秒为单位的
    h = playtime/3600;
    m = (playtime-h*3600)/60;
    s = playtime-h*3600-m*60;

    if(h<10)
        hour = "0" + QString::number(h) + ":";
    else
        hour = QString::number(h) + ":";

    if(m<10)
        min = "0" + QString::number(m) + ":";
    else
        min = QString::number(m) + ":";

    if(s<10)
        sec= "0" + QString::number(s);
    else
        sec = QString::number(s);

    return hour + min + sec;
}

//整理视频
void MediaPlayer::on_pushButton_video_clicked()
{
    QString tempStr = "./start.sh ";

    QString program_folder = QDir::currentPath();
    if(program_folder.isEmpty())
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("请输入工程路径！"));
        return;
    }
    program_folder += "/ ";
    tempStr += program_folder;

    QString video_folder = ui->lineEdit_video->text();
    if(video_folder.isEmpty())
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("请输入视频文件夹路径"));
        return;
    }
    video_folder += "/ ";
    tempStr += video_folder;

    QString cutVideoStart = ui->dateTimeEdit_start->text() + " ";
    QString cutVideoEnd = ui->dateTimeEdit_end->text() + " ";

    QString scutYear = ui->dateTimeEdit_start->sectionText(QDateTimeEdit::YearSection);
    QString ecutYear = ui->dateTimeEdit_end->sectionText(QDateTimeEdit::YearSection);

    QString scutMonth = ui->dateTimeEdit_start->sectionText(QDateTimeEdit::MonthSection);
    QString ecutMonth = ui->dateTimeEdit_end->sectionText(QDateTimeEdit::MonthSection);

    QString scutDay = ui->dateTimeEdit_start->sectionText(QDateTimeEdit::DaySection);
    QString ecutDay= ui->dateTimeEdit_end->sectionText(QDateTimeEdit::DaySection);

#if 1
    if(cutVideoEnd <= cutVideoStart)
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("结束时间不大于开始时间，请重新输入！"));
        return;
    }
    if(QString::compare(scutYear,ecutYear) != 0)
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("输入两个时间年份不相同，请重新输入！"));
        return;
    }

    if(QString::compare(scutMonth,ecutMonth) != 0)
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("输入两个时间月份不相同，请重新输入！"));
        return;
    }

    if(QString::compare(scutDay,ecutDay) != 0)
    {
        QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("输入两个时间不是同一天，请重新输入！"));
        return;
    }
#endif

    tempStr += cutVideoStart;
    tempStr += cutVideoEnd;

    qDebug()<<"tempStr:"<<tempStr;
    command = tempStr;

    Cutpthread *cut = new Cutpthread;
    cut->start();

    QDateTime Start = ui->dateTimeEdit_start->dateTime();
    QDateTime End = ui->dateTimeEdit_end->dateTime();
    int stime = Start.toTime_t();
    int etime = End.toTime_t();
    int fileSeconds = etime - stime;
    qDebug()<<"fileSeconds:"<<fileSeconds;

    int totalSeconds = 170000*0.10 * fileSeconds; //压缩一秒视频大约需要循环170000*1.5次

    QProgressDialog *progressDlg = new QProgressDialog;
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setMinimumDuration(0);//dialog出现需等待的时间
    progressDlg->setWindowTitle("请稍等...");
    progressDlg->setLabelText("正在切割...");
    progressDlg->setCancelButtonText("取消");


    progressDlg->setRange(0,totalSeconds);

    for(int i=0;i<totalSeconds;++i)
    {
        progressDlg->setValue(i);
        if(progressDlg->wasCanceled())
        {
            QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("压缩失败"));
             return;
        }
    }
    //最后1%阻塞3秒
    QThread::msleep(3000);
    progressDlg->setValue(totalSeconds);

    //system(command.toStdString().c_str());

    //实时播放
    if(ui->checkBox->isChecked())
    {
        QString filename = QDir::currentPath() + "/new_video/rtsp-server-0.mp4";
        QFile file(filename);

        qDebug()<<filename;
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(NULL, "Title", "Content", QMessageBox::Ok,
                                     QMessageBox::Ok);
            return;
        }

        if(if_reload)
        {//重新加载视频时，重置以下变量
            delete layout_video;
            delete player;
            delete widget;
            delete timer;
        }
        if_reload = true;

        //实例化三个全局变量
        layout_video = new QVBoxLayout;
        player = new QMediaPlayer;
        widget = new QVideoWidget;
        //设置视频播放区域与Label的边距
        layout_video->setMargin(1);
        //根据label_player尺寸设置播放区域
        widget->resize(ui->label_play_video->size());
        layout_video->addWidget(widget);
        ui->label_play_video->setLayout(layout_video);
        player->setVideoOutput(widget);

        //设置播放器
        player->setMedia(QUrl::fromLocalFile(filename));
        //play_state为true表示播放，false表示暂停
        play_state = true;
        //ui->pushButton_Volume->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider->setRange(0,maxValue);
        //启用播放/暂停按钮，并将其文本设置为“暂停”
        ui->pushButton_Player->setEnabled(true);
        ui->pushButton_Player->setText("暂停");

        timer = new QTimer();
        timer->setInterval(1000);
        timer->start();

        connect(timer,SIGNAL(timeout()),this,SLOT(onTimerOut()));
        connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(setEndTime(qint64)));
        //connect(timer,SIGNAL(timeout()),this,SLOT(setHeadTime()));

        //settime(qint64 playtime);
        //播放器开启
        player->play();
    }
    //压缩视频
    if(ui->checkBox_video->isChecked())
    {
        qDebug()<<"11111111111111111111";
        QDateTime Start = ui->dateTimeEdit_start->dateTime();
        QDateTime End = ui->dateTimeEdit_end->dateTime();
        int stime = Start.toTime_t();
        int etime = End.toTime_t();
        int fileSeconds = etime - stime;
        int totalSeconds = 1.4 * fileSeconds * 170000; //压缩一秒视频大约需要循环170000*1.5次

        CompThread *comp = new CompThread;
        comp->start();
        qDebug()<<comp->currentThreadId();

#if 1

        QProgressDialog *progressDlg = new QProgressDialog;
        progressDlg->setWindowModality(Qt::WindowModal);
        progressDlg->setMinimumDuration(0);//dialog出现需等待的时间
        progressDlg->setWindowTitle("请稍等...");
        progressDlg->setLabelText("正在压缩...");
        progressDlg->setCancelButtonText("取消");


        progressDlg->setRange(0,totalSeconds);

        for(int i=0;i<totalSeconds;++i)
        {
            progressDlg->setValue(i);
            if(progressDlg->wasCanceled())
            {
                QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("压缩失败"));
                 return;
            }
        }
        //最后1%阻塞3秒
        QThread::msleep(3000);
        progressDlg->setValue(totalSeconds);
#endif
    }

}

void MediaPlayer::OnMsgSignal(const std::thread::id &tep)
{
    std::cout<<std::this_thread::get_id()<<std::endl;
}

void MediaPlayer::on_pushButton_reset_clicked()
{
    QString tempStr = "./script/reset.sh";
    std::string command = tempStr.toStdString();
    system(command.c_str());
}

QString MediaPlayer::spetime(QString time)
{
    if(time.length() == 0)
        return 0;

    time = time.insert(4,"-");
    time = time.insert(7,"-");
    time = time.insert(10,"-");
    time = time.insert(13,"-");
    time = time.insert(16,"-");

    return time;
}

int MediaPlayer::get_timespace(QString time)
{
    if(time.isEmpty())
        return 0;

    time = QString::fromStdString(time.toStdString().substr(8,6));
    int h1 = QString::fromStdString(time.toStdString().substr(0,2)).toInt();
    int m1 = QString::fromStdString(time.toStdString().substr(2,2)).toInt();
    int s1 = QString::fromStdString(time.toStdString().substr(4,2)).toInt();

    return h1*3600 + m1*60 + s1;
}

void MediaPlayer::on_checkBox_playtime_clicked()
{

}

void MediaPlayer::on_checkBox_clicked(bool checked)
{

}
