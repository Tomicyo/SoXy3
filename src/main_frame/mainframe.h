#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "QtCore"
#include "QtGui"
#include "notifywin.h"
#include "proxymanager.h"
#include "QList"

class QMovie;
class QPropertyAnimation;
class QParallelAnimationGroup;
class QSystemTrayIcon;

#ifndef NOPOCO
class SogouProxyServer;
#endif

namespace Ui {
class MainFrame;
}

class Config
{
public:

    static Config* getInstance(){
        static Config c;
        return &c;
    }

    virtual void enableAutoStart(bool enable){
        if(enable){
            QString path = QCoreApplication::applicationFilePath();
            path = path.replace("/","\\");
            autorun_setting->setValue("SoXy3", path);
        }
        else{
            if(autorun_setting->contains("SoXy3"))
                autorun_setting->remove("SoXy3");
        }
    }

    virtual void enableProxy(bool e){
        pm->enableProxy(e);
    }

    virtual void switchToProxy(QString host){
        pm->enableProxy(host, true);
    }

private:
    Config(){
        autorun_setting = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\",
                                             QSettings::NativeFormat);
        pm = new ProxyManager;
    }
    ~Config(){
        delete autorun_setting;
        delete pm;
    }

    QSettings *autorun_setting;
    ProxyManager *pm;
};

class MainFrame : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainFrame(QWidget *parent = 0);
    ~MainFrame();
    virtual void addPage(QWidget *w, QIcon ico= QIcon());
    virtual void addPage(QWidget *w, QIcon ico, int level);

public slots:
    virtual void addNotify(QString info);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void deleteNotify();
//    void on_pb_cert_clicked();
    void firstAvaliable();

private:
    /*! [2]Functions
     */
    void startUpSogouServer();
    void loadPlugins();
    void setupUi();
    /*! [1] Members
    */
#ifndef NOPOCO
    SogouProxyServer *internal_server;
#endif
    Ui::MainFrame *ui;
    QSystemTrayIcon *trayIcon;
    QList<NotifyWin *> notify_win;
    QList<QPropertyAnimation*> ani_list;
    QParallelAnimationGroup *ani_grp;

    QPoint mouseMovePos;

    int pre_index;
private slots:
    void popUp(QSystemTrayIcon::ActivationReason a);
    void swipe(int index);
    void on_toolButton_clicked();
};

#endif // MAINFRAME_H
