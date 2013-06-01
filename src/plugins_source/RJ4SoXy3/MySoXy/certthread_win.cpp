/*!
 * Modified by Dso Tsen
 *@ Tsin Studio 2012 ™ NWPU
 *Date :2013 1 12
 *Email: dsotsen@gmail.com
 */

#include "certthread.h"
#include "md5.h"

#include <QProcess>
#include <QTextCodec>
#include <QDebug>
#include <QFile>

#ifdef Q_OS_WIN
#include <IPHlpApi.h>
#elif defined(Q_OS_LINUX)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#define __MAX_SEND_COUNT				3

const uchar CertThread::STANDARD_ADDR[] = {0x01,0x80,0xC2,0x00,0x00,0x03};
const uchar CertThread::RUIJIE_ADDR[] = {0x01,0xD0,0xF8,0x00,0x00,0x03};

static void pcap_handle(uchar *user, const struct pcap_pkthdr *h, const uchar *buf);

CertThread::CertThread(QObject *parent)
    : QThread(parent)
{
    //initPcap();
    m_state = __DISCONNECT;
    m_hPcap = NULL;
    m_dataBuf = NULL;
    m_dataSize = 0;
    memset(m_userName,0,sizeof(m_userName));
    memset(m_password,0,sizeof(m_password));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(sendoooo()));
}

CertThread::~CertThread()
{
    if (m_hPcap != NULL)
        pcap_close(m_hPcap);
    if (m_dataBuf != NULL)
        free(m_dataBuf);
}

void CertThread::printMsg(const uchar *buf, unsigned bufLen)
{
    char *serverMsg;
    unsigned length = buf[0x1b];

    if (length > 0)
    {
        for (serverMsg=(char *)(buf+0x1c); *serverMsg==' '||*serverMsg=='\r'||*serverMsg=='\n'; serverMsg++,length--);	/* 跳过开头的换行符 */
        QByteArray encodedString(serverMsg);
        QTextCodec *codec = QTextCodec::codecForName("gb2312");
        QString string = codec->toUnicode(encodedString);
        emit sendMsg( string );
    }
    if ((length=0x1c+buf[0x1b]+0x69+39) < bufLen)
    {
        serverMsg=(char *)(buf+length);
        if ((unsigned)(buf[length-1]-2) > bufLen-length)
            length = bufLen - length;
        else
            length = buf[length-1]-2;
        for (; *serverMsg==' '||*serverMsg=='\r'||*serverMsg=='\n'; serverMsg++,length--);
        char msg[257];
        strncpy(msg, serverMsg, length);
        msg[length] = '\0';
        QByteArray encodedString(serverMsg);
        QTextCodec *codec = QTextCodec::codecForName("gb2312");
        QString string = codec->toUnicode(encodedString);
        emit sendMsg( string );
    }
}

void CertThread::showMsg(const uchar *buf)
{
    int length = ntohs(*(ushort *)(buf+0x14)) - 5;
    if (length <= 0)
        return;
    char msg[513];
    memcpy(msg, buf+0x17, length);
    msg[length] = '\0';
    QByteArray encodedString(msg);
    QTextCodec *codec = QTextCodec::codecForName("gb2312");
    QString string = codec->toUnicode(encodedString);
    emit sendMsg( string );
}

void CertThread::startCert()
{
    if (!getAddress() || !openAdapter())
        return;

    getSize();
    fillHeader();
    m_timer->start(m_timeout);
//创建认证线程
    start(QThread::NormalPriority);
    qDebug()<< "Already Start";
    return;
}

bool CertThread::getAddress()
{
#ifdef Q_OS_WIN
    PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
    }
    bool bFound = false;
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
    {
        const char *nic = m_nic + (strlen(m_nic) - 38);
        for (PIP_ADAPTER_INFO pAdapter=pAdapterInfo; pAdapter; pAdapter=pAdapter->Next)
        {
            if (strcmp(nic, pAdapter->AdapterName) == 0)
            {
                memcpy(m_localMAC, pAdapter->Address, 6);
                qDebug()<<m_localMAC;
                //转换地址
                m_rip = inet_addr(pAdapter->IpAddressList.IpAddress.String);
                m_mask = inet_addr(pAdapter->IpAddressList.IpMask.String);
                m_gateway = inet_addr(pAdapter->GatewayList.IpAddress.String);
                bFound = true;
                break;
            }
        }
    }
    free(pAdapterInfo);
    if (!bFound)
    {
//获取网络地址失败！
        //mainDlg->MsgBox(LoadString(IDS_ADDRESS_ERROR));
        return false;
    }

    PFIXED_INFO pFixedInfo = (PFIXED_INFO)malloc(sizeof(FIXED_INFO));
    ulOutBufLen = sizeof(FIXED_INFO);
    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pFixedInfo);
        pFixedInfo = (PFIXED_INFO)malloc(ulOutBufLen);
    }
    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR)
        m_dns = inet_addr(pFixedInfo->DnsServerList.IpAddress.String);
    free(pFixedInfo);
#else
    struct ifreq ifr;
    #ifndef SIOCGIFHWADDR	/* BSD、MacOS */
        struct ifaddrs *ifap, *p = NULL;
        struct sockaddr_dl *sdl;
    #endif
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            printf("!! 创建套接字失败!\n");
            return -1;
        }
        strcpy(ifr.ifr_name, m_nic);

    #ifdef SIOCGIFHWADDR
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
            /*goto getMACError*/;
        memcpy(m_localMAC, ifr.ifr_hwaddr.sa_data, 6);
    #else
        if (getifaddrs(&ifap) == 0)
        {
            for (p=ifap; p; p=p->ifa_next)
            {
                if (p->ifa_name && strcmp(p->ifa_name, m_nic)==0)
                {
                    sdl = (struct sockaddr_dl *)p->ifa_addr;
                    memcpy(m_localMAC, sdl->sdl_data + sdl->sdl_nlen, 6);
                    break;
                }
            }
            freeifaddrs(ifap);
        }
        if (p == NULL)
            goto getMACError;
    #endif

//        if (startMode == 0)
//            memcpy(destMAC, STANDARD_ADDR, 6);
//        else if (startMode == 1)
//            memcpy(destMAC, RUIJIE_ADDR, 6);

    #ifndef NO_ARP
        m_gateMAC[0] = 0xFE;
        if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
            printf("!! 在网卡%s上获取IP失败!\n", m_nic);
        else {
            m_rip = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
            if (m_gateway!=0 && (m_startMode%3!=2 || ((u_char *)&m_gateway)[3]!=0x02))
                m_gateMAC[0] = 0xFF;
        }
        if (m_dhcpMode!=0 || m_ip==-1)
            m_ip = m_rip;
    #else
        if (dhcpMode!=0 || ip==-1) {
            if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
                printf("!! 在网卡%s上获取IP失败!\n", nic);
            else
                ip = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
        }
    #endif

        if (m_dhcpMode!=0 || m_mask==-1) {
            if (ioctl(sock, SIOCGIFNETMASK, &ifr) < 0)
                printf("!! 在网卡%s上获取子网掩码失败!\n", m_nic);
            else
                m_mask = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
        }
        close(sock);
#endif
    uchar *ip;
    if (m_dhcpMode==0 && m_ip!=0 && m_ip!=m_rip)
    {
        ip = (uchar *)&m_ip;
        //** 绑定IP:   %d.%d.%d.%d
        //strTemp.Format(LoadString(IDS_BANDIP), ip[0], ip[1], ip[2], ip[3]);
        //mainDlg->Output(strTemp, 2);
    }
    else
        m_ip = m_rip;
    ip = (uchar*)&m_ip;
    uchar *mask = (uchar *)&m_mask;
    uchar *gateway = (uchar *)&m_gateway;
    uchar *dns = (uchar *)&m_dns;
    QString string;
    string.sprintf("Local IP: %d.%d.%d.%d\nMask: %d.%d.%d.%d\nGateWay: %d.%d.%d.%d\nDNS: %d.%d.%d.%d\nLocal MAC: %02x-%02x-%02x-%02x-%02x-%02x",
                   ip[0], ip[1], ip[2], ip[3],
                   mask[0], mask[1], mask[2], mask[3],
                   gateway[0], gateway[1], gateway[2], gateway[3],
                   dns[0], dns[1], dns[2], dns[3],
                   m_localMAC[0], m_localMAC[1], m_localMAC[2], m_localMAC[3]
                   );
    emit sendMsg( string );
    //** 首选DNS:  %d.%d.%d.%d
    //strTemp.Format(LoadString(IDS_LOCALDNS), ip[0], ip[1], ip[2], ip[3]);
    //mainDlg->Output(strTemp, 2);
    return true;
}

bool CertThread::openAdapter()
{
    if (m_hPcap != NULL)
        pcap_close(m_hPcap);
    char buf[PCAP_ERRBUF_SIZE];
    if ((m_hPcap = pcap_open_live(m_nic, 65535, 1, 500, buf)) == NULL)
    {
        //mainDlg->MsgBox(LoadString(IDS_OPEN_ERROR));
        return false;
    }
    struct bpf_program fcode;
    char mac[20];
    sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", m_localMAC[0], m_localMAC[1],
        m_localMAC[2], m_localMAC[3], m_localMAC[4], m_localMAC[5]);
    sprintf(buf, "(ether proto 0x888e and (ether dst %s or ether dst 01:80:c2:00:00:03)"
                      "and not ether src %s) or ether proto 0x0806", mac, mac);
    qDebug()<<mac<<buf;
    if (pcap_compile(m_hPcap, &fcode, buf, 1, 0xffffff) == -1
        || pcap_setfilter(m_hPcap, &fcode) == -1)
    {
        pcap_close(m_hPcap);
        //设置过滤器失败！
        //mainDlg->MsgBox(LoadString(IDS_FILTER_ERROR));
        return false;
    }
    pcap_freecode(&fcode);
    return true;
}

void CertThread::getSize()
{
    if (m_dataBuf != NULL)
        free(m_dataBuf);
    char buf[16];
    memcpy(buf, data.constData(), 16);
    m_readSize = (*(uint *)buf ^ *(uint *)(buf + 8)) + 16;
    m_checkSize = (*(uint *)buf ^ *(uint *)(buf + 12)) + 16;

    m_dataSize = data.size();

    if (m_checkSize<m_readSize || m_dataSize<m_readSize)
        goto fileError;
    m_dataSize = (m_dataSize - m_readSize) / 2 + 0x17;
    if (m_dataSize < 0x80)
        goto fileError;
    m_dataBuf = (uchar *)malloc(m_dataSize);
    return;
fileError:
    if (m_usePackage)
    {
        m_usePackage = false;
        emit sendMsg("Package Error!");
    }
    m_dataSize = 0x80;
    m_dataBuf = (uchar *)malloc(m_dataSize);
}

////////////////////////////////////////////////////////////////////////////////////////
static void pcap_handle(uchar *user, const struct pcap_pkthdr *h, const uchar *buf)
{
    CertThread *proc = (CertThread *)user;

    if (buf[0x0c]==0x88 && buf[0x0d]==0x8e)
    {
        if (memcmp(proc->m_servMAC, buf+6, 6)!=0 && proc->m_startMode>=2)	/* 服务器MAC地址不符 */
            return;
        proc->m_pcapBuf = buf;
        if (buf[0x0F]==0x00 && buf[0x12]==0x01 && buf[0x16]==0x01)	/* 验证用户名 */
        {
            qDebug()<< "Check UserName";
            if (proc->m_startMode < 2)
            {
                memcpy(proc->m_servMAC, buf+6, 6);
                //** 认证MAC:  %02x-%02x-%02x-%02x-%02x-%02x
                //strTemp.Format(LoadString(IDS_SERVMAC), buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
                //mainDlg->Output(strTemp, 2);
                proc->m_startMode += 2;/* 标记为已获取 */
            }
            //mainDlg->m_sServerMsg.Empty();
            if (!proc->m_bCernet && memcmp(buf+0x17, "User name", 9)==0)
            {
                proc->m_bCernet = true;
/*
                CString strFileName = GetAppPath() + _T("Config.ini");
                WritePrivateProfileString(_T("Parameters"), _T("ClientType"), _T("1"), strFileName);
*/
            }
            proc->switchState(CertThread::__IDENTITY);
        }
        else if (buf[0x0F]==0x00 && buf[0x12]==0x01 && buf[0x16]==0x04)	/* 验证密码 */
            proc->switchState(CertThread::__CHALLENGE);
        else if (buf[0x0F]==0x00 && buf[0x12]==0x03)	/* 认证成功 */
        {
            //
            qDebug()<< "Cert Succeed";
            if (!proc->m_bCernet)
            {
                proc->getEchoKey(buf);
                proc->printMsg(buf, h->caplen);
            }
            if (proc->m_dhcpMode==1 || proc->m_dhcpMode==2)	/* 二次认证第一次或者认证后 */
                proc->switchState(CertThread::__DHCP);
            else if (proc->m_bCernet)
                proc->switchState(CertThread::__WAITECHO);
            else
                proc->switchState(CertThread::__ECHO);
        }
        else if (buf[0x0F]==0x00 && buf[0x12]==0x01 && buf[0x16]==0x02)	/* 显示提示信息 */
            proc->showMsg(buf);
        else if (buf[0x0F] == 0x05)	/* 响应在线 */
        {
//            mainDlg->m_sServerMsg.Empty();
            proc->switchState(CertThread::__ECHO);
        }
        else if (buf[0x0F]==0x00 && buf[0x12]==0x04)  /* 认证失败或被踢下线 */
        {
            if (proc->m_state==CertThread::__WAITECHO || proc->m_state==CertThread::__ECHO)
            {
//                mainDlg->Output(LoadString(IDS_CERT_DOWN));
                proc->switchState(CertThread::__START);
            }
            else if (buf[0x1b]!=0 || proc->m_bCernet)
            {
//                mainDlg->Output(LoadString(IDS_CERT_FAILED));
                if (!proc->m_bCernet)
                    proc->printMsg(buf, h->caplen);
                proc->restart();
            }
            else
                proc->switchState(CertThread::__START);
//            mainDlg->m_bTime=false;//add by tsl0922
        }
//        mainDlg->ChangeTrayIcon();
    }
    else if (buf[0x0c] ==0x08 && buf[0x0d]==0x06 && proc->m_bandMac!=0)
    {
        if (*(uint *)(buf+0x1c) == proc->m_gateway)
        {
            if (proc->m_bandMac == 1)
            {
                memcpy(proc->m_gateMAC, buf+0x16, 6);
//                strTemp.Format(LoadString(IDS_GATEMAC), buf[0x16], buf[0x17], buf[0x18], buf[0x19], buf[0x1a], buf[0x1b]);
//                mainDlg->Output(strTemp, 2);
                uchar *ip = (uchar *)&proc->m_gateway;
//Arp
                char __arp_param[128] = {'\0'};
                sprintf( __arp_param, ("-s %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x"), ip[0], ip[1], ip[2], ip[3],
                    buf[0x16], buf[0x17], buf[0x18], buf[0x19], buf[0x1a], buf[0x1b]);
                QProcess::execute("arp", QStringList()<< QString(__arp_param) );

                proc->m_bandMac = 2;
            }
            else if (buf[0x15]==0x02 && *(uint *)(buf+0x26)==proc->m_rip)
            {
                if (memcmp(proc->m_gateMAC, buf+0x16, 6) != 0)
                {
                    //!! ARP欺骗源: %02x-%02x-%02x-%02x-%02x-%02x
//                    mainDlg->m_sServerMsg.Format(LoadString(IDS_ARP_ALERT),
//                        buf[0x16], buf[0x17], buf[0x18], buf[0x19], buf[0x1a], buf[0x1b]);
//                    mainDlg->Output(mainDlg->m_sServerMsg);
//                    mainDlg->ChangeTrayIcon();
                }
            }
			else 
				qDebug()<< "Out of Case";
        }
    }
}

void CertThread::restart()
{
    //m_timer->stop();
    if (m_startMode >= 2) m_startMode -= 2;

    m_state = __START;
    m_sendCount = -1;
    //m_timer->start();
}

void CertThread::run()
{
    if (this->m_dhcpMode == 3)
        this->switchState(CertThread::__DHCP);
    else
        this->switchState(CertThread::__START);

    if (-1 == pcap_loop(this->m_hPcap, -1, pcap_handle, (uchar *)this)) /* 开始捕获数据包 */
    {
        emit sendMsg("End Cert");
    }
    emit sendMsg("End Loop");
	
}
void CertThread::endCert()
{
    switchState(__DISCONNECT);
    pcap_breakloop(m_hPcap);
    //m_timer->stop();
    if (m_bandMac == 2) m_bandMac = 1;
    if (m_dhcpMode >= 4) m_dhcpMode -= 3;
    if (m_startMode >= 2) m_startMode -= 2;


    qDebug()<< "Error End Cert";
}

uchar CertThread::encode(uchar base)	/* 算法，将一个字节的8位颠倒并取反 */
{
    uchar result = 0;
    int i;
    for (i=0; i<8; i++)
    {
        result <<= 1;
        result |= base&0x01;
        base >>= 1;
    }
    return(~result);
}

void CertThread::checkSum()
{
    uchar table[] =
    {
        0x00,0x00,0x21,0x10,0x42,0x20,0x63,0x30,0x84,0x40,0xA5,0x50,0xC6,0x60,0xE7,0x70,
        0x08,0x81,0x29,0x91,0x4A,0xA1,0x6B,0xB1,0x8C,0xC1,0xAD,0xD1,0xCE,0xE1,0xEF,0xF1,
        0x31,0x12,0x10,0x02,0x73,0x32,0x52,0x22,0xB5,0x52,0x94,0x42,0xF7,0x72,0xD6,0x62,
        0x39,0x93,0x18,0x83,0x7B,0xB3,0x5A,0xA3,0xBD,0xD3,0x9C,0xC3,0xFF,0xF3,0xDE,0xE3,
        0x62,0x24,0x43,0x34,0x20,0x04,0x01,0x14,0xE6,0x64,0xC7,0x74,0xA4,0x44,0x85,0x54,
        0x6A,0xA5,0x4B,0xB5,0x28,0x85,0x09,0x95,0xEE,0xE5,0xCF,0xF5,0xAC,0xC5,0x8D,0xD5,
        0x53,0x36,0x72,0x26,0x11,0x16,0x30,0x06,0xD7,0x76,0xF6,0x66,0x95,0x56,0xB4,0x46,
        0x5B,0xB7,0x7A,0xA7,0x19,0x97,0x38,0x87,0xDF,0xF7,0xFE,0xE7,0x9D,0xD7,0xBC,0xC7,
        0xC4,0x48,0xE5,0x58,0x86,0x68,0xA7,0x78,0x40,0x08,0x61,0x18,0x02,0x28,0x23,0x38,
        0xCC,0xC9,0xED,0xD9,0x8E,0xE9,0xAF,0xF9,0x48,0x89,0x69,0x99,0x0A,0xA9,0x2B,0xB9,
        0xF5,0x5A,0xD4,0x4A,0xB7,0x7A,0x96,0x6A,0x71,0x1A,0x50,0x0A,0x33,0x3A,0x12,0x2A,
        0xFD,0xDB,0xDC,0xCB,0xBF,0xFB,0x9E,0xEB,0x79,0x9B,0x58,0x8B,0x3B,0xBB,0x1A,0xAB,
        0xA6,0x6C,0x87,0x7C,0xE4,0x4C,0xC5,0x5C,0x22,0x2C,0x03,0x3C,0x60,0x0C,0x41,0x1C,
        0xAE,0xED,0x8F,0xFD,0xEC,0xCD,0xCD,0xDD,0x2A,0xAD,0x0B,0xBD,0x68,0x8D,0x49,0x9D,
        0x97,0x7E,0xB6,0x6E,0xD5,0x5E,0xF4,0x4E,0x13,0x3E,0x32,0x2E,0x51,0x1E,0x70,0x0E,
        0x9F,0xFF,0xBE,0xEF,0xDD,0xDF,0xFC,0xCF,0x1B,0xBF,0x3A,0xAF,0x59,0x9F,0x78,0x8F,
        0x88,0x91,0xA9,0x81,0xCA,0xB1,0xEB,0xA1,0x0C,0xD1,0x2D,0xC1,0x4E,0xF1,0x6F,0xE1,
        0x80,0x10,0xA1,0x00,0xC2,0x30,0xE3,0x20,0x04,0x50,0x25,0x40,0x46,0x70,0x67,0x60,
        0xB9,0x83,0x98,0x93,0xFB,0xA3,0xDA,0xB3,0x3D,0xC3,0x1C,0xD3,0x7F,0xE3,0x5E,0xF3,
        0xB1,0x02,0x90,0x12,0xF3,0x22,0xD2,0x32,0x35,0x42,0x14,0x52,0x77,0x62,0x56,0x72,
        0xEA,0xB5,0xCB,0xA5,0xA8,0x95,0x89,0x85,0x6E,0xF5,0x4F,0xE5,0x2C,0xD5,0x0D,0xC5,
        0xE2,0x34,0xC3,0x24,0xA0,0x14,0x81,0x04,0x66,0x74,0x47,0x64,0x24,0x54,0x05,0x44,
        0xDB,0xA7,0xFA,0xB7,0x99,0x87,0xB8,0x97,0x5F,0xE7,0x7E,0xF7,0x1D,0xC7,0x3C,0xD7,
        0xD3,0x26,0xF2,0x36,0x91,0x06,0xB0,0x16,0x57,0x66,0x76,0x76,0x15,0x46,0x34,0x56,
        0x4C,0xD9,0x6D,0xC9,0x0E,0xF9,0x2F,0xE9,0xC8,0x99,0xE9,0x89,0x8A,0xB9,0xAB,0xA9,
        0x44,0x58,0x65,0x48,0x06,0x78,0x27,0x68,0xC0,0x18,0xE1,0x08,0x82,0x38,0xA3,0x28,
        0x7D,0xCB,0x5C,0xDB,0x3F,0xEB,0x1E,0xFB,0xF9,0x8B,0xD8,0x9B,0xBB,0xAB,0x9A,0xBB,
        0x75,0x4A,0x54,0x5A,0x37,0x6A,0x16,0x7A,0xF1,0x0A,0xD0,0x1A,0xB3,0x2A,0x92,0x3A,
        0x2E,0xFD,0x0F,0xED,0x6C,0xDD,0x4D,0xCD,0xAA,0xBD,0x8B,0xAD,0xE8,0x9D,0xC9,0x8D,
        0x26,0x7C,0x07,0x6C,0x64,0x5C,0x45,0x4C,0xA2,0x3C,0x83,0x2C,0xE0,0x1C,0xC1,0x0C,
        0x1F,0xEF,0x3E,0xFF,0x5D,0xCF,0x7C,0xDF,0x9B,0xAF,0xBA,0xBF,0xD9,0x8F,0xF8,0x9F,
        0x17,0x6E,0x36,0x7E,0x55,0x4E,0x74,0x5E,0x93,0x2E,0xB2,0x3E,0xD1,0x0E,0xF0,0x1E
    };
    uchar *checkSum = m_dataBuf + 0x15;
    int i, index;
    for (i=0; i<0x15; i++)
    {
        index = checkSum[0] ^ m_dataBuf[i];
        checkSum[0] = checkSum[1] ^ table[index*2+1];
        checkSum[1] = table[index*2];
    }
    for (i=0; i<0x17; i++)
        m_dataBuf[i] = encode(m_dataBuf[i]);
}

void CertThread::fillHeader()
{
    memset(m_dataBuf, 0, m_dataSize);
    m_dataBuf[0x02] = 0x13;
    m_dataBuf[0x03] = 0x11;
    if (m_dhcpMode != 0)
        m_dataBuf[0x04] = 0x01;		/* DHCP位，使用1、不使用0 */
    memcpy(m_dataBuf+0x05, &m_ip, 4);
    memcpy(m_dataBuf+0x09, &m_mask, 4);
    memcpy(m_dataBuf+0x0D, &m_gateway, 4);
    memcpy(m_dataBuf+0x11, &m_dns, 4);
    checkSum();
}

int CertThread::setVendor(uchar type, const uchar *value, int length)
{
    uchar *p = m_dataBuf + 0x46, *end = m_dataBuf + m_dataSize - length - 8;
    while (p <= end)
    {
        if (*p == 0x1a)
            p += 2;
        if (p[4] == type)
        {
            memcpy(p+4+p[5]-length, value, length);
            return 0;
        }
        p += p[5] + 4;
    }
    return -1;
}

int CertThread::readPacket(int type)
{
    uchar dhcp[] = {0x00};
    type %= 2;
    memcpy(m_dataBuf+0x17, (const char*)(data.constData()+m_readSize+(m_dataSize-0x17)*type),  m_dataSize-0x17);
    if (m_dhcpMode == 1)
        dhcp[0] = 0x01;
    setVendor(0x18, dhcp, 1);
    setVendor(0x2D, m_localMAC, 6);
    return 0;

fileError:
    m_usePackage = false;
    //!! 所选数据包无效，改用内置数据认证。
    return -1;
}

void CertThread::fillStartPacket()
{
    if (!m_usePackage)	/* 不使用数据包？ */
    {
        uchar packet[] =
        {
            0x00,0x00,0x13,0x11,0x38,0x30,0x32,0x31,0x78,0x2e,0x65,0x78,0x65,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0x11,0x00,0x28,0x1a,
            0x28,0x00,0x00,0x13,0x11,0x17,0x22,0x91,0x66,0x64,0x93,0x67,0x60,0x65,0x62,0x62,
            0x94,0x61,0x69,0x67,0x63,0x91,0x93,0x92,0x68,0x66,0x93,0x91,0x66,0x95,0x65,0xaa,
            0xdc,0x64,0x98,0x96,0x6a,0x9d,0x66,0x00,0x00,0x13,0x11,0x18,0x06,0x02,0x00,0x00
        };
        memcpy(m_dataBuf+0x17, packet, sizeof(packet));
        if (m_dhcpMode == 1)
            m_dataBuf[0x77] = 0x01;
        else
            m_dataBuf[0x77] = 0x00;
    }
    else if (readPacket(0) == -1)
        fillStartPacket();
}

void CertThread::fillMd5Packet(const uchar *md5Seed)
{
//    	 不使用数据包？
    if (!m_usePackage)
    {
        /* xrgsu的Md5包与Start包有一个字节的差异，若以其他版本为基础，可进一步区别对待 */
        fillStartPacket();
        m_dataBuf[0x3F] = 0x04;
    }
    else if (readPacket(1) == -1 || checkRuijie(md5Seed) == -1)
            fillMd5Packet(md5Seed);
//    	 初始化echoNo
    m_echoNo = 0x0000102B;
}

/*!
  *算法，填充锐捷客户端MD5校验部分
*/
int CertThread::checkRuijie(const uchar *md5Seed)
{
    uchar *ruijie;
    char md5[33];
    uchar table[144], *md5Dig;
    unsigned i, j;

//    CString strTemp;
//    ** 客户端版本:%d.%d 适用:%s 类型:%d
//    strTemp.Format(LoadString(IDS_RUIJIE_VERSION), m_dataBuf[0x3B], m_dataBuf[0x3C],\
//        m_dataBuf[0x3D]?_T("Linux"):_T("Windows"), m_dataBuf[0x3E]);
//    mainDlg->Output(strTemp, 2);


    if (m_readSize<0x810 || ntohs(*(ushort *)(m_dataBuf+0x3B)) < 0x0238)		/* 2.56之前 */
    {
//        !! 该版本没有客户端校验。
//        mainDlg->Output(LoadString(IDS_NO_CLIENTCHECK), 2);
        return 0;
    }

    ruijie = (uchar *)malloc(m_checkSize);

    memcpy(ruijie, data.constData(), m_readSize);

    for (i=16; i<m_readSize; i+=16)	/* 还原数据 */
    {
        for (j=0; j<16; j++)
            ruijie[i+j] ^= ruijie[j];
    }
    memset(ruijie+m_readSize, 0, m_checkSize-m_readSize);
//     算法，填充锐捷客户端MD5校验部分
    j = (m_checkSize - 16) / 8;
    for (i=0; i<8; i++)
    {
        memcpy(ruijie + j * i, md5Seed, 16);
        md5Dig = ComputeHash(ruijie + j * i, j + 16);
        table[18*i] = md5Seed[2*i];
        memcpy(table+18*i+1, md5Dig, 16);
        table[18*i+17] = md5Seed[2*i+1];
    }
    free(ruijie);
    md5Dig = ComputeHash(table, 144);
    for (i=0; i<16; i++)
        sprintf(md5+2*i,"%02x", md5Dig[i]);

//    strTemp = md5;
//    mainDlg->Output(LoadString(IDS_MD5_STRING)+strTemp, 2);

    setVendor(0x17, (uchar *)md5, 32);
    return 0;

fileError:
    m_usePackage = false;
    emit sendMsg("Package Error!");
//    mainDlg->Output(LoadString(IDS_PACKAGE_ERROR), 2);
    return -1;
}

void CertThread::fillEchoPacket()
{
    int i;
    uchar bt1[4], bt2[4];
    *(uint *)bt1 = htonl(m_echoKey + m_echoNo);
    *(uint *)bt2 = htonl(m_echoNo);
    m_echoNo++;
    for (i=0; i<4; i++)
    {
        m_sendPacket[0x18+i] = encode(bt1[i]);
        m_sendPacket[0x22+i] = encode(bt2[i]);
    }
}

void CertThread::getEchoKey(const uchar *buf)
{
    int i, offset = 0x1c + buf[0x1b] + 0x69 + 24;
    m_echoKey = ntohl(*(uint *)(buf + offset));
    uchar *base = (uchar *)(&m_echoKey);
    for (i=0; i<4; i++)
        base[i] = encode(base[i]);
}

uchar *CertThread::checkPass(uchar id, const uchar *md5Seed)
{
    uchar md5Src[80];
    int passLen = strlen(m_password);
    md5Src[0] = id;
    memcpy(md5Src+1, m_password, passLen);
    memcpy(md5Src+1+passLen, md5Seed, 16);
    return ComputeHash(md5Src, passLen+17);
}

int CertThread::switchState(CERT_STATUS type)
{
//     跟上次是同一状态？
    if (m_state == type)
        m_sendCount++;
    else
    {
        m_state = type;
        m_sendCount = 0;
    }
    if (m_autoReconnect!=0 && m_state!=__ECHO)
        qDebug()<< "Close Timer";
        //mainDlg->KillTimer(TEST_TIMER)
        ;
    if (m_sendCount>=__MAX_SEND_COUNT && type!=__ECHO)  /* 超时太多次？ */
    {
        switch (type)
        {
        case __START:
            if (m_dhcpMode == 4)
            {
				qDebug()<<"Cert Sucess";
                //mainDlg->Output(LoadString(IDS_STATE_SUCCESS));
                return switchState(__ECHO);
            }
			qDebug()<< "Start Pack Error";
            //mainDlg->Output(LoadString(IDS_START_ERROR));
            break;
        case __IDENTITY:
			qDebug()<<"User Name Error";
            //mainDlg->Output(LoadString(IDS_USER_ERROR));
            break;
        case __CHALLENGE:
			qDebug()<<"Password Error";
            //mainDlg->Output(LoadString(IDS_PASS_ERROR));
            break;
        case __WAITECHO:
			qDebug()<< "Wait Error";
            //mainDlg->Output(LoadString(IDS_WAIT_ERROR));
            return switchState(__ECHO);
        }
        restart();
        return 0;
    }
    switch (type)
    {
    case __DHCP:
        return getNewIP();
		qDebug()<< "getNewIP";
    case __START:
		qDebug()<< "__START";
        return sendStartPacket();
    case __IDENTITY:
		qDebug()<< "__IDENTITY";
        return sendIdentityPacket();
    case __CHALLENGE:
		qDebug()<< "__CHALLENGE";
        return sendChallengePacket();
    case __WAITECHO:
		qDebug()<< "Wait Echo";
        return waitEchoPacket();
    case __ECHO:
        qDebug()<< "EchoGet......";
        if (m_bandMac != 0){
            qDebug()<< "Go And Send Arp";
            sendArpPacket();
        }
        return sendEchoPacket();
    case __DISCONNECT:
        return sendLogoffPacket();
    }
    return 0;
}

void CertThread::fillEtherAddr(uint protocol)
{
    memset(m_sendPacket, 0, 0x3E8);
    memcpy(m_sendPacket, m_servMAC, 6);
    memcpy(m_sendPacket+0x06, m_localMAC, 6);
    *(uint *)(m_sendPacket+0x0C) = htonl(protocol);
}

int CertThread::sendTestPacket()
{
    uchar testPacket[0x3E8] = {0};
    if (m_startMode % 2 == 0)
        memcpy(testPacket, STANDARD_ADDR, 6);
    else
        memcpy(testPacket, RUIJIE_ADDR, 6);
    memcpy(testPacket+0x06, m_localMAC, 6);
    *(uint *)(testPacket+0x0C) = htonl(0x888E0101);
    memcpy(testPacket+0x12, m_dataBuf, m_dataSize);
    return pcap_sendpacket(m_hPcap, testPacket, 0x3E8);
}

int CertThread::sendStartPacket()
{
    if (m_bCernet)
    {
        if (m_sendCount == 0)
        {
            //mainDlg->Output(LoadString(IDS_STATE_START));
            memcpy(m_sendPacket, STANDARD_ADDR, 6);
            memcpy(m_sendPacket+0x06, m_localMAC, 6);
            *(ushort *)(m_sendPacket+0x0C) = htons(0x888E);
            *(ushort *)(m_sendPacket+0x0E) = htons(0x0101);
            *(ushort *)(m_sendPacket+0x10) = 0;
            memset(m_sendPacket+0x12, 0xa5, 42);
            //mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
        }
        return pcap_sendpacket(m_hPcap, m_sendPacket, 60);
    }
    if (m_sendCount == 0)
    {
        if (m_startMode == 0)
            memcpy(m_servMAC, STANDARD_ADDR, 6);
        else if (m_startMode == 1)
            memcpy(m_servMAC, RUIJIE_ADDR, 6);

        //mainDlg->Output(LoadString(IDS_STATE_START));
        fillStartPacket();
        fillEtherAddr(0x888E0101);
        memcpy(m_sendPacket+0x12, m_dataBuf, m_dataSize);
        //mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
    }
    return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

int CertThread::sendIdentityPacket()
{
    if (m_bCernet)
    {
        int nameLen = strlen(m_userName);
        if (m_sendCount == 0)
        {
            //>> 发送用户名...

            QByteArray encodedString(">>发送用户名");
            QTextCodec *codec = QTextCodec::codecForName("gb2312");
            QString string = codec->toUnicode(encodedString);
            qDebug()<< string;

            *(ushort *)(m_sendPacket+0x0E) = htons(0x0100);
            *(ushort *)(m_sendPacket+0x10) = *(ushort *)(m_sendPacket+0x14) = htons(nameLen+30);
            m_sendPacket[0x12] = 0x02;
            m_sendPacket[0x16] = 0x01;
            m_sendPacket[0x17] = 0x01;
            memcpy(m_sendPacket+0x18, &m_ip, 4);
            memcpy(m_sendPacket+0x1C, &m_mask, 4);
            memcpy(m_sendPacket+0x20, &m_gateway, 4);

            /* memcpy(m_sendPacket+0x24, &m_dns, 4); */

            memset(m_sendPacket+0x24, 0xff, 4);
            memcpy(m_sendPacket+0x28, "03.02.05", 8);
            memcpy(m_sendPacket+0x30, m_userName, nameLen);

//            mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
        }
        m_sendPacket[0x13] = m_pcapBuf[0x13];
        return pcap_sendpacket(m_hPcap, m_sendPacket, nameLen+48);
    }
    if (m_sendCount == 0)
    {
//        mainDlg->Output(LoadString(IDS_STATE_USERNAME));
        fillEtherAddr(0x888E0100);
        int nameLen = strlen(m_userName);
        *(ushort *)(m_sendPacket+0x14) = *(ushort *)(m_sendPacket+0x10) = htons(nameLen+5);
        m_sendPacket[0x12] = 2;
        m_sendPacket[0x13] = m_pcapBuf[0x13];
        m_sendPacket[0x16] = 1;
        memcpy(m_sendPacket+0x17, m_userName, nameLen);
        memcpy(m_sendPacket+0x17+nameLen, m_dataBuf, m_dataSize);
//        mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
    }
    return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

int CertThread::sendChallengePacket()
{
    if (m_bCernet)
    {
        int nameLen = strlen(m_userName);
        if (m_sendCount == 0)
        {
            int md5Len = 0;
            uchar md5Src[50];
            *(ushort *)(m_sendPacket+0x0E) = htons(0x0100);
            *(ushort *)(m_sendPacket+0x10) = *(ushort *)(m_sendPacket+0x14) = htons(nameLen+22);
            m_sendPacket[0x12] = 0x02;
            m_sendPacket[0x13] = m_pcapBuf[0x13];
            m_sendPacket[0x16] = 0x04;
            m_sendPacket[0x17] = 16;
            md5Src[md5Len++] = m_pcapBuf[0x13];
            memcpy(md5Src+md5Len, m_password, strlen(m_password));
            md5Len += strlen(m_password);
            memcpy(md5Src+md5Len, "xxghlmxhzb", 10);
            md5Len += 10;
            memcpy(md5Src+md5Len, m_pcapBuf+0x18, m_pcapBuf[0x17]);
            md5Len += m_pcapBuf[0x17];
            memcpy(m_sendPacket+0x18, ComputeHash(md5Src, md5Len), 16);
            memcpy(m_sendPacket+0x28, m_userName, nameLen);

//            m_timer->setInterval( m_timeout );
        }
        return pcap_sendpacket(m_hPcap, m_sendPacket, nameLen+40);
    }
    if (m_sendCount == 0)
    {
        //>> 发送密码...

        fillMd5Packet(m_pcapBuf + 0x18);
        fillEtherAddr(0x888E0100);
        int nameLen = strlen(m_userName);
        *(ushort *)(m_sendPacket+0x14) = *(ushort *)(m_sendPacket+0x10) = htons(nameLen+22);
        m_sendPacket[0x12] = 2;
        m_sendPacket[0x13] = m_pcapBuf[0x13];
        m_sendPacket[0x16] = 4;
        m_sendPacket[0x17] = 16;
        memcpy(m_sendPacket+0x18, checkPass(m_pcapBuf[0x13],m_pcapBuf+0x18), 16);
        memcpy(m_sendPacket+0x28, m_userName, nameLen);
        memcpy(m_sendPacket+0x28+nameLen, m_dataBuf, m_dataSize);

//        m_timer->setInterval( m_timeout );
    }
    return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

int CertThread::sendEchoPacket()
{
    if (m_bCernet)
    {
        *(ushort *)(m_sendPacket+0x0E) = htons(0x0106);
        *(ushort *)(m_sendPacket+0x10) = 0;
        memset(m_sendPacket+0x12, 0xa5, 42);
        switchState(__WAITECHO);	/* 继续等待 */
        return pcap_sendpacket(m_hPcap, m_sendPacket, 60);
    }
    if (m_sendCount == 0)
    {
        uchar echo[] =
        {
            0x00,0x1E,0xFF,0xFF,0x37,0x77,0x7F,0x9F,0xFF,0xFF,0xD9,0x13,0xFF,0xFF,0x37,0x77,
            0x7F,0x9F,0xFF,0xFF,0xF7,0x2B,0xFF,0xFF,0x37,0x77,0x7F,0x3F,0xFF
        };
        fillStartPacket();
        fillEtherAddr(0x888E01BF);
        memcpy(m_sendPacket+0x10, echo, sizeof(echo));

        qDebug()<< "Set echo timer";

        //mainDlg->SetTimer(SEND_TIMER, m_echoTime, NULL);
        if (m_autoReconnect != 0)
            //mainDlg->SetTimer(TEST_TIMER, m_autoReconnect, NULL)
            qDebug()<< "Must Test";
            ;
    }
    fillEchoPacket();
    qDebug()<< "Send Packet"<< m_sendCount;
    return pcap_sendpacket(m_hPcap, m_sendPacket, 0x2D);
}

int CertThread::sendLogoffPacket()
{
    if (m_bCernet)
    {
        //mainDlg->KillTimer(SEND_TIMER);
        *(ushort *)(m_sendPacket+0x0E) = htons(0x0102);
        *(ushort *)(m_sendPacket+0x10) = 0;
        memset(m_sendPacket+0x12, 0xa5, 42);
        return pcap_sendpacket(m_hPcap, m_sendPacket, 60);
    }
    //mainDlg->KillTimer(SEND_TIMER);
    fillEtherAddr(0x888E0102);
    memcpy(m_sendPacket+0x12, m_dataBuf, m_dataSize);
    return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

void CertThread::sendArpPacket()
{
    uchar arpPacket[0x3C] = {
        0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x01,
        0x08,0x00,0x06,0x04,0x00};

        if (m_dhcpMode == 1 || m_dhcpMode == 2)
            return;
        if (m_bandMac == 2)
        {
            memcpy(arpPacket, m_gateMAC, 6 );
            memcpy(arpPacket+0x06, m_localMAC, 6 );
            arpPacket[0x15]=0x02;
            memcpy(arpPacket+0x16, m_localMAC, 6 );
            memcpy(arpPacket+0x1c, &m_rip, 4 );
            memcpy(arpPacket+0x20, m_gateMAC, 6 );
            memcpy(arpPacket+0x26, &m_gateway, 4 );
            pcap_sendpacket(m_hPcap, arpPacket, 0x3C );
        }
        memset(arpPacket, 0xFF, 6);
        memcpy(arpPacket+0x06, m_localMAC, 6 );
        arpPacket[0x15]=0x01;
        memcpy(arpPacket+0x16, m_localMAC, 6 );
        memcpy(arpPacket+0x1c, &m_rip, 4 );
        memset(arpPacket+0x20, 0, 6 );
        memcpy(arpPacket+0x26, &m_gateway, 4 );
        pcap_sendpacket(m_hPcap, arpPacket, 0x2A );
}

int CertThread::getNewIP()
{
    //mainDlg->KillTimer(SEND_TIMER);
//    QProcess prog;
//    prog.setEnvironment( QProcess::systemEnvironment() );
#if defined(Q_OS_WIN)
//    prog.start("ipconfig", QStringList()<<"/release");
//    prog.waitForFinished(1000);
    //mainDlg->Output(LoadString(IDS_DHCP_START), 2);
//    prog.start("ipconfig", QStringList()<<"/renew");
//    mainDlg->Output(LoadString(IDS_DHCP_END), 2);
#endif

//     标记为已获取，123变为456，5不需再认证
    m_dhcpMode += 3;
    if (!getAddress())
        endCert();
    fillHeader();
    if (m_dhcpMode == 5)
        return switchState(__ECHO);
    return switchState(__START);
}

int CertThread::waitEchoPacket()
{
    if (m_sendCount == 0)
        //mainDlg->SetTimer(SEND_TIMER, m_echoTime, NULL);
        ;
    return 0;
}
