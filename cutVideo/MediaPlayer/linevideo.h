#ifndef LINEVIDEO_H
#define LINEVIDEO_H

#include <QLineEdit>
#include <QMouseEvent>

class LineVideo : public QLineEdit
{
    Q_OBJECT

public:
    LineVideo();

    explicit LineVideo(QWidget* parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();

public slots:
};

#endif // LINEVIDEO_H

