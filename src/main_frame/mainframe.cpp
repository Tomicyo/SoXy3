#include "mainframe.h"
#include "ui_mainframe.h"
#include "qtsvgslideswitch.h"
#ifndef NOPOCO
#include "proxyservice.hpp"
#endif
#include "qnetworkproxy.h"
#include "QSystemTrayIcon"
#include "QWidgetAction"
#include "QMenu"

MainFrame::MainFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainFrame)
{
    setWindowFlags( Qt::FramelessWindowHint );
    setWindowIcon(/*QFileIconProvider().icon(QFileInfo("d:/Allsoft/rj4soxy.exe"))*/
                  QIcon(":/icon/images/soxy.png"));



    ui->setupUi(this);
/*! [1] SetUpUi
 */
    this->setupUi();
/*! [2] startUpSogouServer
 */
    startUpSogouServer();
/*! [3] LoadPlugins
 */
    loadPlugins();
/*! [4] Connect Triggers
 */
    pre_index = ui->tabWidget->currentIndex();
    int total = ui->tabWidget->count();

    ani_grp = new QParallelAnimationGroup;
    for(int  i=0; i<total; i++) {
        QPropertyAnimation *ani = new QPropertyAnimation(ui->tabWidget->widget(i), "geometry");
        ani->setEasingCurve(QEasingCurve::OutCirc);
        ani_list<<ani;
        ani_grp->addAnimation(ani);
    }
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(swipe(int)));
}

MainFrame::~MainFrame()
{
    Config::getInstance()->enableProxy(false);
#ifndef NOPOCO
    internal_server->stop();
    delete internal_server;
#endif
    delete ui;
}

void MainFrame::addPage(QWidget *w, QIcon ico)
{
    ui->tabWidget->addTab(w,ico," ");
}

void MainFrame::addPage(QWidget *w, QIcon ico, int level)
{
    ui->tabWidget->insertTab(level, w, ico, " ");
}

void MainFrame::addNotify(QString info)
{
    notify_win << (new NotifyWin(0, info));
    connect(notify_win.last(), SIGNAL(iGonnaDie()), this, SLOT(deleteNotify()));
    connect(notify_win.first(), SIGNAL(firstAvaliableSignal()), this, SLOT(firstAvaliable()));
    notify_win.last()->show();
}

void MainFrame::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseMovePos != QPoint(0, 0))
    {
     move(geometry().x() + event->globalPos().x() - mouseMovePos.x(), geometry().y() + event->globalPos().y() - mouseMovePos.y());
     mouseMovePos = event->globalPos();
    }
}

void MainFrame::mousePressEvent(QMouseEvent *event)
{
    mouseMovePos = event->globalPos();
}

void MainFrame::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMovePos = QPoint(0, 0);
}

void MainFrame::deleteNotify()
{
    notify_win.first()->close();
    notify_win.first()->deleteLater();
    notify_win.removeFirst();
}

void MainFrame::firstAvaliable()
{
    qWarning("First Ava");
    NotifyWin::iCount = 0;
    qWarning("%d", NotifyWin::iCount);
}

void MainFrame::startUpSogouServer()
{
#ifndef NOPOCO
    ThreadPool::defaultPool().addCapacity(128 - ThreadPool::defaultPool().capacity());

    internal_server = new SogouProxyServer;
    internal_server->setProxyPort(2013);
    internal_server->start();

    QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "127.0.0.1", 2013));
#endif
}

void MainFrame::loadPlugins()
{
    QDir current = QDir::currentPath();
    current.cd("plugins");

    typedef QWidget* (*Instance)(QWidget *);
    typedef int (*Level)();

    QSettings setting("setting.ini",QSettings::IniFormat);
    if(QFile::exists("setting.ini")){
        setting.beginGroup("plugins");
        foreach(QString t, setting.childKeys()){
            if(setting.value(t).toBool()){
                Instance s = (Instance)QLibrary::resolve(current.absolutePath()+"/"+t,
                                                       "Instance");
                Level level = (Level)QLibrary::resolve(current.absolutePath()+"/"+t,
                                                       "level");
                if(s){
                    QWidget *w = s(this);
                    if(level){
                        addPage(w, w->windowIcon(), level());
                        ui->tabWidget->setCurrentIndex(level());
                    }
                    else
                        addPage(w, w->windowIcon());
                }
            }
        }
    }
    else{
        setting.beginGroup("plugins");
        foreach(QString ext, current.entryList(QStringList()<<"*.ext",
                                                 QDir::Files))
        {
            Instance s = (Instance)QLibrary::resolve(current.absolutePath()+"/"+ext,
                                                   "Instance");
            Level level = (Level)QLibrary::resolve(current.absolutePath()+"/"+ext,
                                                   "level");

            setting.setValue(ext,true);

            if(s){
                QWidget *w = s(this);
                if(level){
                    addPage(w, w->windowIcon(), level());
                    ui->tabWidget->setCurrentIndex(level());
                }
                else
                    addPage(w, w->windowIcon());
            }
        }
    }
}

void MainFrame::setupUi()
{
    setWindowOpacity(0.95);

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background,QBrush(QPixmap("resource/background.jpg").scaled(this->size())));
    this->setPalette(palette);
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(bmp.rect(), 10, 10);
    setMask(bmp);

/*! [1] TrayIcon
 */
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icon/images/soxy.png"));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(popUp(QSystemTrayIcon::ActivationReason)));

    QAction *exit_action = new QAction(QIcon(":/icon/images/soxy.png"), tr("Exit"), this);
    connect(exit_action, SIGNAL(triggered()), qApp, SLOT(quit()));

//    QtSvgSlideSwitch *slide_switch =new QtSvgSlideSwitch(this);
//    slide_switch->setSkin("MetallicBrush");

//    QWidgetAction *switch__ = new QWidgetAction(this);
//    slide_switch->setMaximumSize(40, 20);
//    switch__->setDefaultWidget(slide_switch);

    QMenu *ppm = new QMenu;
    ppm->addAction(exit_action);
    trayIcon->setContextMenu(ppm);
    trayIcon->show();

    mouseMovePos = QPoint(0, 0);
}

void MainFrame::popUp(QSystemTrayIcon::ActivationReason a)
{
    if(a == QSystemTrayIcon::Trigger)
        this->showNormal();
}

void MainFrame::swipe(int index)
{
    qDebug() << ui->tabWidget->currentIndex()<< pre_index;
    if(ani_grp->state() != QAbstractAnimation::Running){
        ui->tabWidget->widget(index)->setVisible(true);
        QRect cur_geo = ui->tabWidget->currentWidget()->geometry();
        ani_list[
               index]->setDuration(100);
        ani_list[index]->setEndValue( cur_geo );
        if(index < pre_index)
        {
    /*! [Easing -->]*/
            ani_list[index]->setStartValue(QRect(QPoint(cur_geo.x()-cur_geo.width(), cur_geo.y()),
                                               cur_geo.size()));
        }
        else
        {
    /*! [Easing <--]*/
            ani_list[index]->setStartValue(QRect(QPoint(cur_geo.x()+cur_geo.width(), cur_geo.y()),
                                               cur_geo.size()));
        }
        ani_grp->start();
    }
    else
    {
        ani_grp->stop();
    }
    pre_index = index;
}


void MainFrame::on_toolButton_clicked()
{
    this->hide();
}
