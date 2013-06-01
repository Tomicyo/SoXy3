#ifndef RJWIDGET_H
#define RJWIDGET_H

#include <QWidget>
#include <QProcess>
#include "QDebug"
#include "QApplication"
#include "certthread.h"

namespace Ui {
class RJWidget;
}

class MainFrame;
class RJWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit RJWidget(QWidget *parent = 0);
//    RJWidget(MainFrame *main_frame = 0);
    ~RJWidget();
public slots:
    void beginCert(QString user, QString pass, QString netcard, QString resource);
    void endCert();
    void remsetting();
private slots:
    void passToQMLxnetcard(int index);
    void getMsg(const QString &msg);

private:
    void setupUi();
    void parseSetting();

    Ui::RJWidget *ui;
    CertThread *cert_thread;
    QObject *item;
    MainFrame *m;
};

#endif // RJWIDGET_H
