#ifndef PROXYMANAGER_H
#define PROXYMANAGER_H

class QObject;
class QProcess;
class QSettings;
class MainFrame;
class QString;
#ifdef LIB
    #define DEC __declspec(dllimport)
#else
    #define DEC __declspec(dllexport)
#endif

class DEC ProxyManager
{
public:
    ProxyManager();
    ~ProxyManager();
    virtual void enableProxy(QString host, bool enable);
    virtual void enableProxy(bool enable);
    virtual QString getOverridelist();
    virtual void setOverridelist(QString override_list);
private:
    QSettings *ie_setting;
    QProcess *sogou_backend;
};

#endif // PROXYMANAGER_H
