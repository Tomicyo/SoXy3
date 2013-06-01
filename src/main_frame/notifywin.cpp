#include "notifywin.h"
#include "ui_notifywin.h"
#include <QDesktopWidget>
#include <QPainter>
#include <QDebug>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

int NotifyWin::iCount = 0;

NotifyWin::NotifyWin(QWidget *parent, QString info) :
    QWidget(parent),
    ui(new Ui::NotifyWin)
{
    setWindowFlags( Qt::FramelessWindowHint|Qt::Tool );
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setWindowOpacity(0.75);
    back.load(":/images/background.png");
    life_timer = new QTimer();
    avar = qApp->desktop()->availableGeometry();

    part = avar.height()/height();
    index = -1;
    ui->textBrowser->setPlainText(info);
    connect(life_timer, SIGNAL(timeout()), this, SLOT(timeOut()));
    life_timer->start(500*3);
    animation_geometry = new QPropertyAnimation(this, "geometry");
    animation_geometry->setDuration(500);
//    if(firstAvaliable)
//        iCount = 0;
    animation_geometry->setStartValue(QRect(avar.width(), 10+(iCount%part)*height(), width(), height()));
    animation_geometry->setEndValue(QRect(avar.width()-width(), 10+(iCount%part)*height(), width(), height()));
    animation_geometry->setEasingCurve(QEasingCurve::InOutQuad);
    animation_geometry->start();
    iCount ++;

    if (iCount % part == 1)
        index = 0;
}

NotifyWin::~NotifyWin()
{
    if( index == 0)
        emit firstAvaliableSignal();
    qWarning("Im killed");
//    delete life_timer;
//    life_timer = NULL;
    delete ui;

    delete ani_group;
    //    animation = NULL;
}

void NotifyWin::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()),
                      back,
                      QRect(0, 0, back.width(), back.height()));
}


void NotifyWin::timeOut()
{
    delete animation_geometry;
    if(	windowOpacity()> 0.5 ){
        animation_geometry = new QPropertyAnimation(this, "geometry");
        animation_geometry->setDuration(500);
        animation_geometry->setEndValue(QRect(avar.width()-width(), avar.height()/2 - height()/2, width()/2, height()/2));
        animation_geometry->setEasingCurve(QEasingCurve::InCubic);

        animation_opacity = new QPropertyAnimation(this, "windowOpacity");
        //connect(animation, SIGNAL(finished()), this, SLOT(timeOut()));
        animation_opacity->setDuration(500);
        animation_opacity->setStartValue(qreal(0.75));
        animation_opacity->setEndValue(qreal(0.0));
        animation_opacity->setEasingCurve(QEasingCurve::InCubic);

        ani_group = new QParallelAnimationGroup;
        ani_group->addAnimation(animation_geometry);
        ani_group->addAnimation(animation_opacity);
        connect( ani_group, SIGNAL(finished()), this , SLOT(timeOut()));
        ani_group->start();
    }
    else
    {
        life_timer->stop();
        emit iGonnaDie();
    }
}
