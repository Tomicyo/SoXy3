#include "rjwidget.h"
#include "ui_rjwidget.h"
#include "qdeclarativeview.h"
#include "qdebug.h"
#include "qicon.h"
#include "QDeclarativeContext"
#include "qsettings.h"
#include "pcap_dy.h"

#include "mainframe.h"

RJWidget::RJWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RJWidget)
{
    setupUi();

    m = static_cast<MainFrame*>(parent);
    connect(item, SIGNAL(echo(QString)), m, SLOT(addNotify(QString)));

    cert_thread = new CertThread(this);
    connect(item, SIGNAL(begincert(QString,QString,QString,QString)), this, SLOT(beginCert(QString,QString,QString,QString)));
    connect(item, SIGNAL(endcert()), this, SLOT(endCert()));
    connect(item, SIGNAL(rememsetting()), this, SLOT(remsetting()));
    connect(cert_thread, SIGNAL(sendMsg(QString)),(MainFrame*)parent, SLOT(addNotify(QString)));

    ui->setupUi(this);
    QSettings *setting = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\",
                                       QSettings::NativeFormat);
    foreach(QString entry, setting->childGroups())
    {
        if(setting->value(entry+"/Connection/PnPInstanceId").toString().left(3) == "PCI")
        {
            qDebug()<<entry<<"name:"<<setting->value(entry+"/Connection/Name").toString();

            ui->cbox_nic->addItem( setting->value(entry+"/Connection/Name").toString(), entry);
        }
    }
    connect(ui->cbox_nic, SIGNAL(currentIndexChanged(int)), this, SLOT(passToQMLxnetcard(int)));

    parseSetting();

    this->setStyleSheet("\
                             background-color: rgba(69, 69, 69,32);\
                             border-width: 1px;\
                             border-radius: 8px;");
}

RJWidget::~RJWidget()
{
    QProcess::execute("taskkill /F /IM python.exe /T");
    delete ui;
    delete cert_thread;
}

void RJWidget::beginCert(QString user, QString pass, QString netcard, QString resource)
{
    //UserInfo
    strcpy(cert_thread->m_userName, user.toUtf8().data());
    strcpy(cert_thread->m_password, pass.toUtf8().data());
    QString m_nic = "\\Device\\NPF_"+netcard;
    memcpy(cert_thread->m_nic, m_nic.toLocal8Bit().data(), 60);

    //认证配置
    cert_thread->m_bandMac = 0;

    cert_thread->m_startMode = 1;
    //DHCP Mode:不使用，二次认证，认证后，认证前
    cert_thread->m_dhcpMode = 2;

    cert_thread->m_timeout = 2 * 60 * 1000;

    cert_thread->m_echoTime = 30 * 1000;
    cert_thread->m_autoReconnect = 1;

    //数据包选项
    cert_thread->m_usePackage = false;
    Config::getInstance()->enableProxy(false);

    if(resource == "Free")
    {
        cert_thread->m_usePackage = true;
        QResource package(":/package");
        cert_thread->data =
                package.isCompressed() ? qUncompress(QByteArray((char*)package.data(),package.size())):
                                                 QByteArray((char*)package.data(),package.size());
        Config::getInstance()->enableProxy(true);
    }
    //认证状态
    cert_thread->m_bCernet = false;

    cert_thread->startCert();
}

void RJWidget::endCert()
{
    cert_thread->endCert();
    Config::getInstance()->enableProxy(false);
}

void RJWidget::remsetting()
{
    m->addNotify("Remeber RJ Settings");

    QObject *user = item->findChild<QObject*>("ID_Input");
    QObject *pass = item->findChild<QObject*>("PassWord");

    QSettings setting("setting.ini",QSettings::IniFormat);
    setting.beginGroup("RJ4SoXy3");
    setting.setValue("UserName", user->property("text").toString());
    setting.setValue("Password",
             pass->property("text").toByteArray().toBase64()
            );

    setting.setValue("NetCard", item->property("xnetcard").toString());
    setting.endGroup();
}

void RJWidget::passToQMLxnetcard(int index)
{
    item->setProperty("xnetcard",
                ui->cbox_nic->itemData(index));
}

void RJWidget::getMsg(const QString &msg)
{
    qDebug()<< msg;
}

void RJWidget::setupUi()
{
    setWindowIcon(QIcon(":/img/RJ.png"));
    QDeclarativeView *component = new QDeclarativeView(this);
    component->setSource(QUrl("qrc:/AuthPage.qml"));
    component->setGeometry(0, 0, 552, 281);
    item = (QObject*)component->rootObject();
}

void RJWidget::parseSetting()
{
    QSettings rjsetting("setting.ini",QSettings::IniFormat);
    if(QFile::exists("setting.ini")){
        rjsetting.beginGroup("RJ4SoXy3");
        QString userName = rjsetting.value("UserName").toString();
        QByteArray password = rjsetting.value("Password").toByteArray();

        QString netcard = rjsetting.value("NetCard").toString();
        item->setProperty("xnetcard", netcard);

        QObject *user = item->findChild<QObject*>("ID_Input");
        user->setProperty("text", userName);

        QObject *pass = item->findChild<QObject*>("PassWord");
        pass->setProperty("text", QByteArray::fromBase64(password));

        QObject *remempass = item->findChild<QObject*>("remempass");
        if(!userName.isEmpty())
            remempass->setProperty("state","checked");

        ui->cbox_nic->setCurrentIndex(ui->cbox_nic->findData(netcard));
    }
}

