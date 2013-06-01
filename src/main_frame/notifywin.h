#ifndef NOTIFYWIN_H
#define NOTIFYWIN_H

#include <QWidget>
class QPropertyAnimation;
class QParallelAnimationGroup;

namespace Ui {
class NotifyWin;
}

class NotifyWin : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyWin(QWidget *parent = 0, QString info = "No Info!");
    ~NotifyWin();
    static int iCount;
protected:
    void paintEvent(QPaintEvent *);

signals:
    void iGonnaDie();
    void firstAvaliableSignal();
private slots:
    void timeOut();
private:
    Ui::NotifyWin *ui;
    QPropertyAnimation     *animation_geometry;
    QPropertyAnimation     *animation_opacity;
    QParallelAnimationGroup *ani_group;
    QTimer *life_timer;
    QImage back;
    QRect avar;
    int part, index;
};

#endif // NOTIFYWIN_H
