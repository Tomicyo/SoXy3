#include "proxymanager.h"
#include "mainframe.h"
#include "QtCore"
#include "QtGui"
#include "Windows.h"
#include <WinInet.h>

ProxyManager::ProxyManager()
{
    ie_setting = new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\",
                               QSettings::NativeFormat);
    sogou_backend = new QProcess;
    sogou_backend->setEnvironment(QProcess::systemEnvironment());
}

ProxyManager::~ProxyManager()
{
    delete ie_setting;
    delete sogou_backend;
}

void ProxyManager::enableProxy(QString host, bool enable)
{
    if(enable){
        ie_setting->setValue("ProxyServer",host);
        ie_setting->setValue("ProxyEnable",1);
    }
    else{
        ie_setting->setValue("ProxyEnable",0);
    }
    InternetSetOptionA(0, 39, 0, 0);
}

void ProxyManager::enableProxy(bool enable)
{
    if(enable){
        ie_setting->setValue("ProxyServer","127.0.0.1:1998");
        ie_setting->setValue("ProxyEnable",1);
        if( sogou_backend->state()!=QProcess::Running)
            sogou_backend->start("python backends/SogouProxy4NPuers.py");
    }
    else{
        ie_setting->setValue("ProxyEnable",0);
    }
    InternetSetOptionA(0, 39, 0, 0);
}

QString ProxyManager::getOverridelist()
{
    return ie_setting->value("ProxyOverride").toString();
}

void ProxyManager::setOverridelist(QString override_list)
{
    ie_setting->setValue("ProxyOverride", override_list);
    InternetSetOptionA(0, 39, 0, 0);
}
