#include "pcap_dy.h"
#ifndef PCAP_HPP
#define PCAP_HPP
_pcap_close pcap_close;
_pcap_open_live pcap_open_live;
_pcap_compile pcap_compile;
_pcap_setfilter pcap_setfilter;
_pcap_freecode pcap_freecode;
_pcap_loop pcap_loop;
_pcap_sendpacket pcap_sendpacket;
_pcap_freealldevs pcap_freealldevs;
_pcap_findalldevs pcap_findalldevs;
_pcap_breakloop pcap_breakloop;

void initPcap(){
    QLibrary libpcap("wpcap.dll");
    if(libpcap.isLoaded())
    {
        qDebug() << "Failed to Load Pcap";
        exit(100);
    }
    else{
        pcap_close = (_pcap_close)libpcap.resolve("pcap_close");
        pcap_open_live = (_pcap_open_live)libpcap.resolve("pcap_open_live");
        pcap_compile = (_pcap_compile)libpcap.resolve("pcap_compile");
        pcap_setfilter = (_pcap_setfilter)libpcap.resolve("pcap_setfilter");
        pcap_freecode = (_pcap_freecode)libpcap.resolve("pcap_freecode");
        pcap_loop = (_pcap_loop)libpcap.resolve("pcap_loop");
        pcap_sendpacket = (_pcap_sendpacket)libpcap.resolve("pcap_sendpacket");

        pcap_freealldevs = (_pcap_freealldevs)libpcap.resolve("pcap_freealldevs");
        pcap_findalldevs = (_pcap_findalldevs)libpcap.resolve("pcap_findalldevs");
        pcap_breakloop = (_pcap_breakloop)libpcap.resolve("pcap_breakloop");

    }
}
#endif
