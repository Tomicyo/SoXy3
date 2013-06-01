/*!
 * Modified by Dso Tsen
 *@ Tsin Studio 2012 ™ NWPU
 *Date :2013 1 12
 *Email: dsotsen@gmail.com
 */
#ifndef PROCESS_H
#define PROCESS_H
#define MAX_PATH   256
#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include "pcap_dy.h"
#define  _UNICODE
#include <tchar.h>
#else
#include <wchar.h>
#endif

#include "pcap.h"

#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QResource>

class CertThread : public QThread
{
    Q_OBJECT
public:
    CertThread(QObject *parent = 0);
    ~CertThread();

//    void pcap_handle(uchar *user, const struct pcap_pkthdr *h, const uchar *buf);
    void showMsg(const uchar *buf);
    void printMsg(const uchar *buf, unsigned bufLen);

    enum CERT_STATUS{
        __DISCONNECT,
        __START,
        __IDENTITY,
        __CHALLENGE,
        __ECHO,
        __DHCP,
        __WAITECHO
    };

    static QString getStatus(CERT_STATUS stat)
    {
        switch(stat)
        {
        case __DISCONNECT: return QString("Status: Disconnect"); break;
        case __START: return QString("Status: Start"); break;
        case __IDENTITY: return QString("Status: Identify"); break;
        case __CHALLENGE: return QString("Status: Challenge"); break;
        case __ECHO: return QString("Status: Echo"); break;
        case __DHCP: return QString("Status: DHCP"); break;
        case __WAITECHO: return QString("Status: WaitEcho"); break;
        default:
            break;
        }
        return QString("Status: Unknown");
    }

//     开始认证
    void startCert();
//     停止认证
    void endCert();
//     改变状态
    int switchState(CERT_STATUS type);

    int sendTestPacket();

    void getEchoKey(const uchar *buf);
//    重启认证
    void restart();
//     认证状态
    volatile CERT_STATUS m_state;
/*!
 * User Variant
 * Username--Password
 * ServerMac--GateMac
 * Netcard Name
 * Timeout--EchoTime--AutoReconnect--Restart
 * Certification Mode
 */
// 用户名、密码
    char m_userName[66], m_password[64];

    uchar m_servMAC[6], m_gateMAC[6];

    uint m_ip, m_rip, m_gateway;
// 网卡名
    char m_nic[60];

    /*!
       重连间隔
   */
    unsigned m_timeout, m_echoTime, m_autoReconnect;
// 组播方式、DHCP方式
    unsigned m_startMode, m_dhcpMode;
// 绑定网关MAC？
    int m_bandMac;
// 使用数据包
    bool m_usePackage;
// 数据包
#ifdef C_STD
    wchar_t m_package[MAX_PATH];
#endif
    QByteArray data;
// pcap句柄
    pcap_t *m_hPcap;

    const uchar *m_pcapBuf;

    bool m_bCernet;

protected:
    void run();
public slots:
    void sendoooo(){
        qDebug()<<getStatus(m_state);
        if (this->switchState(m_state) == -1)
        {
            qDebug()<<"Already Quit";
            this->endCert();
        }
        qDebug()<<"EndfOOOO";
    }
signals:
    void sendMsg(QString msg);
private:
    QTimer *m_timer;

    static const uchar STANDARD_ADDR[];
    static const uchar RUIJIE_ADDR[];
    uchar m_localMAC[6];
    uint m_mask, m_dns;
    uchar m_sendPacket[1000];
    uchar *m_dataBuf;
    unsigned m_dataSize;
    unsigned m_readSize, m_checkSize;
    uint m_echoKey, m_echoNo;
    unsigned m_sendCount;

/*!
    CWinThread *m_pCertThread;	//认证线程
*/
// 获取网络地址
    bool getAddress();
//	 打开网卡
    bool openAdapter();
//     获取大小
    void getSize();

    uchar encode(uchar base);
    void checkSum();
    void fillHeader();
    int setVendor(uchar type, const uchar *value, int length);
    int readPacket(int type);
    void fillStartPacket();
    void fillMd5Packet(const uchar *md5Seed);
    int checkRuijie(const uchar *md5Seed);

    void fillEchoPacket();
    uchar *checkPass(uchar id, const uchar *md5Seed);

    void fillEtherAddr(uint protocol);

    int sendStartPacket();
    int sendIdentityPacket();
    int sendChallengePacket();
    int sendEchoPacket();
    int sendLogoffPacket();
    void sendArpPacket();

    int getNewIP();
    int waitEchoPacket();
};
#endif // PROCESS_H
