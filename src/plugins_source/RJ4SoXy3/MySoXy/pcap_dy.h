#ifndef PCAP_HPP
#define PCAP_HPP
#include <QLibrary>
#include <QDebug>
#include <QString>
#if defined(Q_OS_WIN)
    #include <WinSock2.h>
    #pragma comment(lib,"WS2_32.lib")
#endif

struct pcap_pkthdr {
    struct timeval ts;	/* time stamp */
    uint caplen;	/* length of portion present */
    uint len;	/* length this packet (off wire) */
};

struct pcap_addr {
    struct pcap_addr *next;
    struct sockaddr *addr;		/* address */
    struct sockaddr *netmask;	/* netmask for that address */
    struct sockaddr *broadaddr;	/* broadcast address for that address */
    struct sockaddr *dstaddr;	/* P2P destination address for that address */
};

struct bpf_insn {
    ushort	code;
    uchar 	jt;
    uchar 	jf;
    int k;
};

struct pcap_if {
    struct pcap_if *next;
    char *name;		/* name to hand to "pcap_open_live()" */
    char *description;	/* textual description of interface, or NULL */
    struct pcap_addr *addresses;
    uint flags;	/* PCAP_IF_ interface flags */
};
typedef pcap_if pcap_if_t;
struct bpf_program {
    uint bf_len;
    struct bpf_insn *bf_insns;
};
typedef struct pcap pcap_t;

typedef void (*_pcap_close)(pcap_t *);

typedef pcap_t* (*_pcap_open_live)(const char *, int, int, int, char *);

typedef int	(*_pcap_compile)(pcap_t *, struct bpf_program *, char *, int,
        uint);

typedef int	(*_pcap_setfilter)(pcap_t *, struct bpf_program *);

typedef void (*_pcap_freecode)(struct bpf_program *);
typedef int	(*_pcap_loop)(pcap_t *, int, void*, uchar *);
typedef int	(*_pcap_sendpacket)(pcap_t *, const uchar *, int);
typedef void	(*_pcap_freealldevs)(pcap_if_t *);
typedef int	(*_pcap_findalldevs)(pcap_if_t **, char *);
typedef void (*_pcap_breakloop)(pcap_t *);


extern _pcap_close pcap_close;
extern _pcap_open_live pcap_open_live;
extern _pcap_compile pcap_compile;
extern _pcap_setfilter pcap_setfilter;
extern _pcap_freecode pcap_freecode;
extern _pcap_loop pcap_loop;
extern _pcap_sendpacket pcap_sendpacket;
extern _pcap_freealldevs pcap_freealldevs;
extern _pcap_findalldevs pcap_findalldevs;
extern _pcap_breakloop pcap_breakloop;


#define PCAP_ERRBUF_SIZE 256

void initPcap();

#endif // PCAP_HPP
