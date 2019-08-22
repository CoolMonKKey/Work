#include "linevideo.h"

#include <QFileDialog>
#include <iostream>

using namespace std;

LineVideo::LineVideo(QWidget *parent) :
    QLineEdit(parent)
{

}

//重写mousePressEvent事件,检测事件类型是不是点击了鼠标左键
void LineVideo::mousePressEvent(QMouseEvent *event)
{
    //如果单击了就触发clicked信号
    if (event->button() == Qt::LeftButton)
    {
        //触发clicked信号
        emit clicked();
    }
    //将该事件传给父类处理
    QLineEdit::mousePressEvent(event);
}

void LineVideo::clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Directory"), "", tr("video (*.mp4 *.mpeg *.h264)"));
}
