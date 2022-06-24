/* Automation Studio Generated Header File, Format Version 1.00 */
/* do not change */
#ifndef ETHSOCK_H_
#define ETHSOCK_H_
#define _WEAK	__attribute__((__weak__))

#include <bur/plc.h>
#include <bur/plctypes.h>



/* Constants */
_WEAK const unsigned short ERR_ETH_2BIG = 27107;
_WEAK const unsigned short ERR_ETH_ACCESS = 27113;
_WEAK const unsigned short ERR_ETH_ADDRINUSE = 27148;
_WEAK const unsigned short ERR_ETH_ADDRNOTAVAIL = 27149;
_WEAK const unsigned short ERR_ETH_AFNOTSUP = 27147;
_WEAK const unsigned short ERR_ETH_AGAIN = 27111;
_WEAK const unsigned short ERR_ETH_ALREADY = 27169;
_WEAK const unsigned short ERR_ETH_BADF = 27109;
_WEAK const unsigned short ERR_ETH_BADMSG = 27177;
_WEAK const unsigned short ERR_ETH_BASE = 27100;
_WEAK const unsigned short ERR_ETH_BUSY = 27116;
_WEAK const unsigned short ERR_ETH_CANCELED = 27172;
_WEAK const unsigned short ERR_ETH_CHILD = 27110;
_WEAK const unsigned short ERR_ETH_CONNABORTED = 27153;
_WEAK const unsigned short ERR_ETH_CONNREFUSED = 27161;
_WEAK const unsigned short ERR_ETH_CONNRESET = 27154;
_WEAK const unsigned short ERR_ETH_DEADLK = 27133;
_WEAK const unsigned short ERR_ETH_DESTADDRREQ = 27140;
_WEAK const unsigned short ERR_ETH_DOM = 27137;
_WEAK const unsigned short ERR_ETH_EXIST = 27117;
_WEAK const unsigned short ERR_ETH_FAULT = 27114;
_WEAK const unsigned short ERR_ETH_FBIG = 27127;
_WEAK const unsigned short ERR_ETH_FD_SETSIZE = 27200;
_WEAK const unsigned short ERR_ETH_HOSTDOWN = 27167;
_WEAK const unsigned short ERR_ETH_HOSTUNREACH = 27165;
_WEAK const unsigned short ERR_ETH_INPROGRESS = 27168;
_WEAK const unsigned short ERR_ETH_INTR = 27104;
_WEAK const unsigned short ERR_ETH_INVAL = 27122;
_WEAK const unsigned short ERR_ETH_IO = 27105;
_WEAK const unsigned short ERR_ETH_ISCONN = 27156;
_WEAK const unsigned short ERR_ETH_ISDIR = 27121;
_WEAK const unsigned short ERR_ETH_LOOP = 27164;
_WEAK const unsigned short ERR_ETH_MAX = 27181;
_WEAK const unsigned short ERR_ETH_MFILE = 27124;
_WEAK const unsigned short ERR_ETH_MLINK = 27131;
_WEAK const unsigned short ERR_ETH_MSGSIZE = 27136;
_WEAK const unsigned short ERR_ETH_NAMETOOLONG = 27126;
_WEAK const unsigned short ERR_ETH_NETDOWN = 27162;
_WEAK const unsigned short ERR_ETH_NETRESET = 27152;
_WEAK const unsigned short ERR_ETH_NETUNREACH = 27151;
_WEAK const unsigned short ERR_ETH_NFILE = 27123;
_WEAK const unsigned short ERR_ETH_NOBUFS = 27155;
_WEAK const unsigned short ERR_ETH_NODATA = 27178;
_WEAK const unsigned short ERR_ETH_NODEV = 27119;
_WEAK const unsigned short ERR_ETH_NOENT = 27102;
_WEAK const unsigned short ERR_ETH_NOEXEC = 27108;
_WEAK const unsigned short ERR_ETH_NOLCK = 27134;
_WEAK const unsigned short ERR_ETH_NOMEM = 27112;
_WEAK const unsigned short ERR_ETH_NOMGR = 27248;
_WEAK const unsigned short ERR_ETH_NOMSG = 27180;
_WEAK const unsigned short ERR_ETH_NOPROTOOPT = 27142;
_WEAK const unsigned short ERR_ETH_NOSPC = 27128;
_WEAK const unsigned short ERR_ETH_NOSR = 27174;
_WEAK const unsigned short ERR_ETH_NOSTR = 27175;
_WEAK const unsigned short ERR_ETH_NOSYS = 27171;
_WEAK const unsigned short ERR_ETH_NOTBLK = 27166;
_WEAK const unsigned short ERR_ETH_NOTCONN = 27157;
_WEAK const unsigned short ERR_ETH_NOTDIR = 27120;
_WEAK const unsigned short ERR_ETH_NOTEMPTY = 27115;
_WEAK const unsigned short ERR_ETH_NOTSOCK = 27150;
_WEAK const unsigned short ERR_ETH_NOTSUP = 27135;
_WEAK const unsigned short ERR_ETH_NOTTY = 27125;
_WEAK const unsigned short ERR_ETH_NXIO = 27106;
_WEAK const unsigned short ERR_ETH_OPNOTSUP = 27145;
_WEAK const unsigned short ERR_ETH_PERM = 27101;
_WEAK const unsigned short ERR_ETH_PFNOTSUP = 27146;
_WEAK const unsigned short ERR_ETH_PIPE = 27132;
_WEAK const unsigned short ERR_ETH_PROTO = 27176;
_WEAK const unsigned short ERR_ETH_PROTONOTSUP = 27143;
_WEAK const unsigned short ERR_ETH_PROTOTYPE = 27141;
_WEAK const unsigned short ERR_ETH_RANGE = 27138;
_WEAK const unsigned short ERR_ETH_ROFS = 27130;
_WEAK const unsigned short ERR_ETH_SHUTDOWN = 27158;
_WEAK const unsigned short ERR_ETH_SOCKNOTSUP = 27144;
_WEAK const unsigned short ERR_ETH_SPIPE = 27129;
_WEAK const unsigned short ERR_ETH_SRCH = 27103;
_WEAK const unsigned short ERR_ETH_TIME = 27179;
_WEAK const unsigned short ERR_ETH_TIMEDOUT = 27160;
_WEAK const unsigned short ERR_ETH_TOOMANYREFS = 27159;
_WEAK const unsigned short ERR_ETH_TXTBSY = 27163;
_WEAK const unsigned short ERR_ETH_UNKNOWN = 27249;
_WEAK const unsigned short ERR_ETH_WOULDBLOCK = 27170;
_WEAK const unsigned short ERR_ETH_XDEV = 27118;
_WEAK const signed long ERR_SOCKET = -1;
_WEAK const signed long ETH_AF_INET = 2;
_WEAK const unsigned long ETH_ASYNC = 2147772029U;
_WEAK const signed long ETH_FD_SETSIZE = 256;
_WEAK const signed long ETH_FIOGETOWN = 1074030204;
_WEAK const unsigned long ETH_FIONBIO = 2147772030U;
_WEAK const unsigned long ETH_FIOSETOWN = 2147772028U;
_WEAK const signed long ETH_INADDR_ANY = 0;
_WEAK const unsigned char ETH_INET_ADDR_LEN = 18;
_WEAK const signed long ETH_IPPROTO_TCP = 6;
_WEAK const signed long ETH_IPPROTO_UDP = 17;
_WEAK const signed long ETH_SOCK_DGRAM = 2;
_WEAK const signed long ETH_SOCK_RAW = 3;
_WEAK const signed long ETH_SOCK_STREAM = 1;
_WEAK const signed long ETH_SOL_SOCKET = 65535;
_WEAK const signed long ETH_SOMAXCONN = 5;
_WEAK const signed long ETH_SO_ACCEPTCONN = 2;
_WEAK const signed long ETH_SO_BROADCAST = 32;
_WEAK const signed long ETH_SO_DONTROUTE = 16;
_WEAK const signed long ETH_SO_ERROR = 4103;
_WEAK const signed long ETH_SO_KEEPALIVE = 8;
_WEAK const signed long ETH_SO_LINGER = 128;
_WEAK const signed long ETH_SO_OOBINLINE = 256;
_WEAK const signed long ETH_SO_RCVBUF = 4098;
_WEAK const signed long ETH_SO_RCVLOWAT = 4100;
_WEAK const signed long ETH_SO_RCVTIMEO = 4102;
_WEAK const signed long ETH_SO_REUSEADDR = 4;
_WEAK const signed long ETH_SO_SNDBUF = 4097;
_WEAK const signed long ETH_SO_SNDLOWAT = 4099;
_WEAK const signed long ETH_SO_SNDTIMEO = 4101;
_WEAK const signed long ETH_SO_TYPE = 4104;


/* Datatypes */
typedef struct ETH_sockaddr_typ
{
	unsigned short sin_family;
	unsigned short sin_port;
	unsigned long sin_addr;
	unsigned char sin_zero[8];
} ETH_sockaddr_typ;

typedef struct ETH_fd_set_typ
{
	signed long fds_bits[8];
} ETH_fd_set_typ;

typedef struct ETH_timeval_typ
{
	signed long tv_sec;
	signed long tv_usec;
} ETH_timeval_typ;



/* Datatypes of function blocks */


/* Prototyping of functions and function blocks */
unsigned long inet_addr(unsigned long pString);
unsigned short inet_ntoa_b(unsigned long ipaddr, unsigned long pString);
unsigned short EthGetError();
signed long sock_select(signed long width, unsigned long pReadFds, unsigned long pWriteFds, unsigned long pExceptFds, unsigned long pTimeOut);
signed long close(signed long s);
signed long ioctl(signed long s, signed long cmd, unsigned long arg);
signed long shutdown(signed long s, signed long how);
signed long sendto(signed long s, unsigned long buf, signed long buflen, signed long flags, unsigned long to, signed long tolen);
signed long send(signed long s, unsigned long buf, signed long buflen, signed long flags);
signed long recvfrom(signed long s, unsigned long buf, signed long buflen, signed long flags, unsigned long from, unsigned long fromlen);
signed long recv(signed long s, unsigned long buf, signed long buflen, signed long flags);
signed long listen(signed long s, signed long backlog);
signed long getpeername(signed long s, unsigned long name, unsigned long namelen);
signed long getsockname(signed long s, unsigned long name, unsigned long namelen);
signed long setsockopt(signed long s, signed long level, signed long optname, unsigned long optval, signed long optlen);
signed long getsockopt(signed long s, signed long level, signed long optname, unsigned long optval, unsigned long optlen);
signed long connect(signed long s, unsigned long name, signed long namelen);
signed long bind(signed long s, unsigned long name, signed long namelen);
signed long accept(signed long s, unsigned long addr, unsigned long addrlen);
signed long socket(signed long domain, signed long type, signed long protocol);
unsigned short fd_set(signed long n, unsigned long p);
unsigned short fd_clr(signed long n, unsigned long p);
unsigned short fd_isset(signed long n, unsigned long p);
unsigned short fd_zero(unsigned long p);



/* structure and symbol definitions only for m68k-Targets */


/*---------------------------------------------------------------------*/
/* Protocols Supported by pNA+                                         */
/*---------------------------------------------------------------------*/
#define TCP             0x6         /* TCP protocol */
#define UDP             0x11        /* UDP protocol */
#define IPPROTO_TCP     0x6         /* TCP protocol */
#define IPPROTO_UDP     0x11        /* UDP protocol */

/*---------------------------------------------------------------------*/
/* Flags Used by Send/Recieve Calls                                    */
/*---------------------------------------------------------------------*/
#define MSG_OOB         0x1         /* process out-of-band data */
#define MSG_PEEK        0x2         /* peek at incoming message */
#define MSG_DONTROUTE   0x4         /* send without using routing tables */

/*---------------------------------------------------------------------*/
/* Maximum Length of the Vectors used in sendmsg() and recvmsg()       */
/*---------------------------------------------------------------------*/
#define MSG_MAXIOVLEN   0x10

/***********************************************************************/
/* Control Commands for ioctl() to control socket action               */
/***********************************************************************/
#define FIOREAD         0x4004667F  /* Get bytes in socket read buffer */
#define SIOCATMARK      0x40047307  /* Get out of band data mark */

/*----------------------------------------------------------------------*/
/* Definitions for Routing entries                                      */
/*----------------------------------------------------------------------*/
#define SIOCADDRT       0x8034720A  /* add route */
#define SIOCDELRT       0x8034720B  /* delete route */

/*----------------------------------------------------------------------*/
/* Definitions for NI parameters                                        */
/*----------------------------------------------------------------------*/
#define SIOCSIFADDR     0x8014690C  /* set interface ip address */
#define SIOCGIFADDR     0xC014690D  /* get interface ip address */
#define SIOCSIFDSTADDR  0x8014690E  /* set point-to-point dest. address */
#define SIOCGIFDSTADDR  0xC014690F  /* get point-to-point dest. address */
#define SIOCSIFFLAGS    0x80146910  /* set interface flags */
#define SIOCGIFFLAGS    0xC0146911  /* get interface flags */
#define SIOCGIFBRDADDR  0xC0146912  /* get interface broadcast addr */
#define SIOCSIFBRDADDR  0x80146913  /* set interface broadcast addr */
#define SIOCGIFCONF     0xC0086914  /* get interface configuration list */
#define SIOCGIFNETMASK  0xC0146915  /* get interface net addr mask */
#define SIOCSIFNETMASK  0x80146916  /* set interface net addr mask */
#define SIOCGIFMTU      0xC0146917  /* get interface MTU */
#define SIOCSIFMTU      0X80146918  /* set interface MTU */

/*----------------------------------------------------------------------*/
/* Definitions for ARP entries                                          */
/*----------------------------------------------------------------------*/
#define SIOCSARP        0x8024691E  /* set arp entry */
#define SIOCGARP        0xC024691F  /* get arp entry */
#define SIOCDARP        0x80246920  /* delete arp entry */

/*----------------------------------------------------------------------*/
/* Definitions for Interface Group MIB variables                        */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGIFNUMBER       0x40016D01 /* get interface number */
#define SIOCGIFTABLE        0x40016D02 /* get Interface table */
#define SIOCGIFINDEX        0x40016D03 /*  get NI index */
#define SIOCGIFDESCR        0x40016D04 /* Get ni descr */
#define SIOCGIFTYPE         0x40016D05 /* Get NI type */
#define SIOCGIFMTUNIT       0x40016D06 /* Get NI mtu */
#define SIOCGIFSPEED        0x40016D07 /* Get NI speed */
#define SIOCGIFPHYSADDRESS  0x40016D08 /* Get ni Phy. Address */
#define SIOCGIFADMINSTATUS  0x40016D09 /* Get ni admin status */
#define SIOCGIFOPERSTATUS   0x40016D0A /* Get NI operatioanl status */
#define SIOCGIFLASTCHANGE   0x40016D0B /* Get last change in status */
#define SIOCGIFINOCTETS     0x40016D0C /* Get no of octets rcvd. */
#define SIOCGIFINUCASTPKTS  0x40016D0D /* Get no. of unicast pkts rcvd  */
#define SIOCGIFINNUCASTPKTS 0x40016D0E /* Get no. of multi. pkts rcvd */
#define SIOCGIFINDISCARDS   0x40016D0F /* Get no of pckts discarded */
#define SIOCGIFINERRORS     0x40016D10 /* Get no of error pckts */
#define SIOCGIFINUNKNOWNPROTOS 0x40016D11 /* No of pckts for unk. protocol */
#define SIOCGIFOUTOCTETS    0x40016D12 /* No of octets rcvd */
#define SIOCGIFOUTUCASTPKTS 0x40016D13 /* No of unicast pkts sent */
#define SIOCGIFOUTNUCASTPKTS 0x40016D14 /* No of multi. pkts sent */
#define SIOCGIFOUTDISCARDS  0x40016D15 /* No of out pkts discarded */
#define SIOCGIFOUTERRORS    0x40016D16 /* No of outbounds pkts errors */
#define SIOCGIFOUTQLEN      0x40016D17 /* Length of output Q length */
#define SIOCGIFSPECIFIC     0x40016D18 /* NI specific parameter */

/*----------------------------------------------------------------------*/
/* set command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCSIFADMINSTATUS  0x80016D09 /* set Interface admin. status */

/*----------------------------------------------------------------------*/
/* Definitions for IP Group MIB variables                               */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGIPFORWARDING   0x40026D01 /* IP forwarding var.   */
#define SIOCGIPDEFAULTTTL   0x40026D02 /* IP def. Time-to-Live var */
#define SIOCGIPINRECEIVES   0x40026D03 /* IP dgrams rec. from below */
#define SIOCGIPINHDRERRORS  0x40026D04 /* IP dgrams format errs drops */
#define SIOCGIPINADDRERRORS 0x40026D05 /* IP dgrams misdelivery drops */
#define SIOCGIPFORWDATAGRAMS 0x40026D06 /* IP dgrams forwarded */
#define SIOCGIPINUNKNOWNPROTOS 0x40026D07 /* IP unknown proto. dgrams */
#define SIOCGIPINDISCARDS   0x40026D08 /* IP disc. dgrms for resources */
#define SIOCGIPINDELIVERS   0x40026D09 /* IP dgrams delivered above */
#define SIOCGIPOUTREQUESTS  0x40026D0A /* IP dgrams sent out */
#define SIOCGIPOUTDISCARDS  0x40026D0B /* IP out dgrams discarded */
#define SIOCGIPOUTNOROUTES  0x40026D0C /* IP dgrams drops for no routes */
#define SIOCGIPREASMTIMEOUT 0x40026D0D /* IP retrans. queue timeout */
#define SIOCGIPREASMREQDS   0x40026D0E /* IP frags. needing reass. */
#define SIOCGIPREASMOKS     0x40026D0F /* IP frags. reassembled */
#define SIOCGIPREASMFAILS   0x40026D10 /* IP frag reassembly fails */
#define SIOCGIPFRAGOKS      0x40026D11 /* IP dgrams fragmented */
#define SIOCGIPFRAGFAILS    0x40026D12 /* IP dgrams fragmentation fails */
#define SIOCGIPFRAGCREATES  0x40026D13 /* IP frags. created */
#define SIOCGIPROUTINGDISCARDS 0x40026D14 /* IP Routing ent. discarded */

/*----------------------------------------------------------------------*/
/* SET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCSIPFORWARDING   0x80026D01 /* IP forwarding variable */
#define SIOCSIPDEFAULTTTL   0x80026D02 /* IP Defautl time-to-live */

/*----------------------------------------------------------------------*/
/* Definitions for IP NI address table                                  */
/* GET commands definitions                                             */
/*----------------------------------------------------------------------*/
#define SIOCGIPADDRTABLE    0x40036D01 /* IP Address table */
#define SIOCGIPADDRENTADDR  0x40036D03 /* IP Address of the addr entry */
#define SIOCGIPADDRENTIFINDEX 0x40036D04 /* NI number of the interface */
#define SIOCGIPADDRENTNETMASK 0x40036D05 /* Netmask of the NI */
#define SIOCGIPADDRENTBCASTADDR 0x40036D06 /* IP Broadcast Address of the NI*/
#define SIOCGIPADDRENTREASMMAXSIZE 0x40036D07 /* IP max. reassm. size */

/*----------------------------------------------------------------------*/
/* Definitions for IP route table                                       */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGIPROUTETABLE   0x40046D01 /* IP routing table */
#define SIOCGIPROUTEDEST    0x40046D03 /* Route destination */
#define SIOCGIPROUTEIFINDEX 0x40046D04 /* Interface Index of the route */
#define SIOCGIPROUTENEXTHOP 0x40046D05 /* Next hop of the route */
#define SIOCGIPROUTETYPE    0x40046D06 /* Type of the route */
#define SIOCGIPROUTEPROTO   0x40046D07 /* Protocol used by the route */
#define SIOCGIPROUTEAGE     0x40046D08 /* Age of the Route  */
#define SIOCGIPROUTEMASK    0x40046D09 /* Mask used by the route */

/*----------------------------------------------------------------------*/
/* SET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCSIPROUTEDEST    0x80046D03 /* Route destination */
#define SIOCSIPROUTENEXTHOP 0x80046D05 /* Next hop of the route */
#define SIOCSIPROUTETYPE    0x80046D06 /* Type of the route */

/*----------------------------------------------------------------------*/
/* Definitions for IP NET-TO-MEDIA table                                */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGIPNETTOMEDIATABLE 0x40056D01 /* IP Net-to-Media table */
#define SIOCGIPNETTOMEDIAIFINDEX 0x40056D03 /* Index of the Entry */
#define SIOCGIPNETTOMEDIAPHYSADDRESS 0x40056D04 /* Physical add. of entry */
#define SIOCGIPNETTOMEDIANETADDRESS 0x40056D05 /* IP address of entry   */
#define SIOCGIPNETTOMEDIATYPE 0x40056D06 /* Type of the entry     */

/*----------------------------------------------------------------------*/
/* SET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCSIPNETTOMEDIAPHYSADDRESS 0x80056D04 /* Physical add. of entry */
#define SIOCSIPNETTOMEDIANETADDRESS 0x80056D05 /* IP address of entry   */
#define SIOCSIPNETTOMEDIATYPE 0x80056D06

/*----------------------------------------------------------------------*/
/* Definitions for ICMP Group MIB variables                             */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGICMPINMSGS     0x40066D01 /* ICMP msgs. received */
#define SIOCGICMPINERRORS   0x40066D02 /* ICMP msgs. with format errs */
#define SIOCGICMPINDESTUNREACHS 0x40066D03 /* ICMP dest. Unreach. msgs rcvd*/
#define SIOCGICMPINTIMEEXCDS 0x40066D04 /* ICMP time excds. msgs. rcvd. */
#define SIOCGICMPINPARAMPROBS 0x40066D05 /* ICMP param. prob. msgs rcvd */
#define SIOCGICMPINSRCQUENCHS 0x40066D06 /*ICMP src. quench msgs rcvd */
#define SIOCGICMPINREDIRECTS 0x40066D07 /* ICMP src quenchs rcvd */
#define SIOCGICMPINECHOS    0x40066D08 /* ICMP echo msgs rcvd */
#define SIOCGICMPINECHOREPS 0x40066D09 /* ICMP echo reply msgs rcvd */
#define SIOCGICMPINTIMESTAMPS 0x40066D0A /* ICMP time stamp msgs rcvd */
#define SIOCGICMPINTIMESTAMPREPS 0x40066D0B /* ICMP time stp rep msgs rcvd */
#define SIOCGICMPINADDRMASKS 0x40066D0C /* ICMP address masks msgs rcvd */
#define SIOCGICMPINADDRMASKREPS 0x40066D0D /*ICMP addr masks reps msgs rcvd */
#define SIOCGICMPOUTMSGS    0x40066D0E /* ICMP messages sent */
#define SIOCGICMPOUTERRORS  0x40066D0F /* ICMP out error msgs. */
#define SIOCGICMPOUTDESTUNREACHS 0x40066D10 /* ICMP dest unreach msgs sent */
#define SIOCGICMPOUTTIMEEXCDS 0x40066D11 /* ICMP time exceed msgs sent */
#define SIOCGICMPOUTPARAMPROBS 0x40066D12 /* ICMP param prob. msgs sent */
#define SIOCGICMPOUTSRCQUENCHS 0x40066D13 /* ICMP src quench msgs sent */
#define SIOCGICMPOUTREDIRECTS 0x40066D14 /* ICMP redirect msgs sent */
#define SIOCGICMPOUTECHOS   0x40066D15 /* ICMP echo msgs sent */
#define SIOCGICMPOUTECHOREPS 0x40066D16 /* ICMP echo reply msgs sent */
#define SIOCGICMPOUTTIMESTAMPS 0x40066D17 /* ICMP time stamps msgs sent */
#define SIOCGICMPOUTTIMESTAMPREPS 0x40066D18 /* ICMP time stps reps sent */
#define SIOCGICMPOUTADDRMASKS 0x40066D19 /* ICMP addr mask msgs sent */
#define SIOCGICMPOUTADDRMASKREPS 0x40066D1A /* ICMP addr mask rep msgs sent */

/*----------------------------------------------------------------------*/
/* Definitions for TCP Group MIB variables                              */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGTCPRTOALGORITHM 0x40076D01 /* TCP retransmission algorithm */
#define SIOCGTCPRTOMIN      0x40076D02 /* TCP min. retrans. timeout */
#define SIOCGTCPRTOMAX      0x40076D03 /* TCP max. retrans. timeout */
#define SIOCGTCPMAXCONN     0x40076D04 /* TCP max. simul. connections */
#define SIOCGTCPACTIVEOPENS 0x40076D05 /* TCP active opens */
#define SIOCGTCPPASSIVEOPENS 0x40076D06 /* TCP number of passive opens */
#define SIOCGTCPATTEMPTFAILS 0x40076D07 /* TCP no. of failed conn. attempts*/
#define SIOCGTCPESTABRESETS 0x40076D08 /* TCP no. connections reset */
#define SIOCGTCPCURRESTAB   0x40076D09 /* TCP no. of curr. connections */
#define SIOCGTCPINSEGS      0x40076D0A /* TCP no. of segs. received */
#define SIOCGTCPOUTSEGS     0x40076D0B /* TCP no. of segs. sent */
#define SIOCGTCPRETRANSSEGS 0x40076D0C /* TCP no. of segs retransmitted */
#define SIOCGTCPCONNTABLE   0x40076D0D /* TCP connection table */
#define SIOCGTCPCONNSTATE   0x40076D0E /* TCP connection entry */
#define SIOCGTCPINERRS      0x40076D0F /* TCP no. of segs. disc. due to errs*/
#define SIOCGTCPOUTRSTS     0x40076D10 /* TCP no. of reset segs. sent */

/*----------------------------------------------------------------------*/
/* SET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCSTCPCONNSTATE  0x80076D0E  /* TCP connection entry */

/*----------------------------------------------------------------------*/
/* Definitions for UDP MIB variables                                    */
/* GET command definitions                                              */
/*----------------------------------------------------------------------*/
#define SIOCGUDPINDATAGRAMS 0x40086D01 /* UDP dgrams delivered */
#define SIOCGUDPNOPORTS    0x40086D02 /* UDP dgrams rcvd for unkown ports*/
#define SIOCGUDPINERRORS   0x40086D03 /* UDP dgrams dropped to format errs*/
#define SIOCGUDPOUTDATAGRAMS 0x40086D04 /* UDP dgrms sent from appls. */
#define SIOCGUDPTABLE      0x40086D05 /* UDP table */
/*----------------------------------------------------------------------*/
/* Definitions for MBUF statistics                                      */
/*----------------------------------------------------------------------*/
#define SIOCGMBUFSTAT      0x40FF7301 /* Mbuf statistics */

/*---------------------------------------------------------------------*/
/* Control Commands for shutdown() to control socket action            */
/*---------------------------------------------------------------------*/
#define FREAD           0x00        /* Shutdown read buffer */
#define FWRITE          0x01        /* Shutdown write buffer */

/*---------------------------------------------------------------------*/
/* Asynchronous signals generated by pNA                               */
/*---------------------------------------------------------------------*/
#define SIGIO           0x40000000  /* Signal I/O on socket */
#define SIGPIPE         0x20000000  /* Signal PIPE on socket */
#define SIGURG          0x10000000  /* Signal Urgent data on socket */

/*---------------------------------------------------------------------*/
/* Control commands for pNA+ chng_route()                              */
/*---------------------------------------------------------------------*/
#define PNA_ADDROUTE    0x0         /* Add a route */
#define PNA_DELROUTE    0x1         /* Delete route */

/*---------------------------------------------------------------------*/
/* Definitions for type of route                                       */
/*---------------------------------------------------------------------*/
#define RT_NETWORK      0x0         /* Network Route */
#define RT_HOST         0x1         /* Host Route */

/***********************************************************************/
/* Definitions of bits in internet address integers.                   */
/***********************************************************************/
/*---------------------------------------------------------------------*/
/* Class A Network                                                     */
/*---------------------------------------------------------------------*/
#define IN_CLASSA(i)     (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET    0xFF000000
#define IN_CLASSA_NSHIFT 0x18
#define IN_CLASSA_HOST   0x00FFFFFF
#define IN_CLASSA_MAX    0x80

/*---------------------------------------------------------------------*/
/* Class B Network                                                     */
/*---------------------------------------------------------------------*/
#define IN_CLASSB(i)     (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET    0xFFFF0000
#define IN_CLASSB_NSHIFT 0x10
#define IN_CLASSB_HOST   0x0000FFFF
#define IN_CLASSB_MAX    0x10000

/*---------------------------------------------------------------------*/
/* Class C Network                                                     */
/*---------------------------------------------------------------------*/
#define IN_CLASSC(i)     (((long)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET    0xFFFFFF00
#define IN_CLASSC_NSHIFT 0x8
#define IN_CLASSC_HOST   0x000000FF

/*---------------------------------------------------------------------*/
/* Broadcast and Default Address Definitions                           */
/*---------------------------------------------------------------------*/
#define INADDR_BROADCAST (unsigned long)0xFFFFFFFF

/*----------------------------------------------------------------------*/
/* Generic Socket address                                               */
/*----------------------------------------------------------------------*/
struct sockaddr
  {
  short sa_family;
  char  sa_data[14];
  };

/*---------------------------------------------------------------------*/
/* Structure used for manipulating linger option.                      */
/*---------------------------------------------------------------------*/
struct linger
    {
    int l_onoff;        /* option on/off */
    int l_linger;       /* linger time */
    };

/***********************************************************************/
/* Structures for sendmsg() and recvmsg()                              */
/***********************************************************************/

/*---------------------------------------------------------------------*/
/* IO vector structure for sendmsg() and recvmsg()                     */
/*---------------------------------------------------------------------*/
struct iovec
    {
    char *iov_base;     /* base address */
    int iov_len;        /* buffer length */
    };

/*---------------------------------------------------------------------*/
/* Message header for sendmsg() and recvmsg()                          */
/*---------------------------------------------------------------------*/
struct msghdr
    {
    char *msg_name;             /* optional address */
    int msg_namelen;            /* size of address */
    struct iovec *msg_iov;      /* scatter/gather array */
    int msg_iovlen;             /* # elements in msg_iov */
    char *msg_accrights;        /* access rights */
    int msg_accrightslen;       /* length of the rights buffer */
    };

/***********************************************************************/
/* Definitions for select().  Select uses bit masks of file descrip-   */
/* tors in longs.  These macros manipulate such bit fields.            */
/***********************************************************************/
#define NFDBITS         0x20        /* bits per mask */

/*---------------------------------------------------------------------*/
/* Timeval structure used in select()                                  */
/*---------------------------------------------------------------------*/
struct timeval
    {
    long  tv_sec;       /* no. of seconds */
    long  tv_usec;      /* no. of micro seconds */
    };

/*----------------------------------------------------------------------*/
/* mbstat structure used in ioctl() call to get mbuf stat               */
/*----------------------------------------------------------------------*/
struct mbstat
    {
    unsigned long m_mbufs;        /* mbufs obtained from page pool */
    unsigned long m_clusters;     /* clusters obtained from page pool */
    unsigned long m_clfree;       /* free clusters */
    unsigned long m_wait;         /* times waited for space */
    unsigned long m_drops;        /* times failed to find space */
    unsigned short m_mtypes[256]; /* type specific mbuf allocations */
    };

/*----------------------------------------------------------------------*/
/* Types of mbufs.  Used internally by pNA                              */
/*----------------------------------------------------------------------*/
#define MT_FREE     0             /* Free type */
#define MT_DATA     1             /* Data type */
#define MT_HEADER   2             /* Packet header type */
#define MT_PCB      4             /* protocol control block type */
#define MT_SONAME   8             /* socket name */

/*----------------------------------------------------------------------*/
/* rtentry structure used in ioctl() call for getting routes from       */
/* pNA routing table                                                    */
/*----------------------------------------------------------------------*/
struct rtentry
    {
    struct sockaddr rt_dst;       /* key to access the route*/
    struct sockaddr rt_gateway;   /* route gateway */
    unsigned short rt_flags;      /* host/net */
    };

#define RTF_HOST    0x1           /* host entry (net otherwise) */
#define RTF_GATEWAY 0x2           /* destination is a gateway */

/*----------------------------------------------------------------------*/
/* ifreq structure used in ioctl() call for getting NI characteristics  */
/*----------------------------------------------------------------------*/
struct ifreq
    {
    long ifr_ifno;
    union
        {
        struct sockaddr ifru_addr;
        struct sockaddr ifru_dstaddr;
        struct sockaddr ifru_broadaddr;
        unsigned short  ifru_flags;
        int             ifru_mtu;
        char            *ifru_data;
        } ifr_ifru;
#define ifr_addr        ifr_ifru.ifru_addr      /* address */
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr   /* other end of p-to-p link */
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr /* broadcast address */
#define ifr_flags       ifr_ifru.ifru_flags     /* flags */
#define ifr_mtu         ifr_ifru.ifru_mtu       /* mtu */
#define ifr_data        ifr_ifru.ifru_data      /* for use by interface */
    };

/*----------------------------------------------------------------------*/
/* ifconf structure used in ioctl() call for getting NI configuration   */
/* using SIOCGIFCONF command                                            */
/*----------------------------------------------------------------------*/
struct ifconf
    {
    int  ifc_len;         /* size of associated buffer */
    union
        {
        char *ifcu_buf;
        struct ifreq *ifcu_req;
        } ifc_ifcu;
#define ifc_buf ifc_ifcu.ifcu_buf       /* buffer address */
#define ifc_req ifc_ifcu.ifcu_req       /* array of structures returned */
    };

/*----------------------------------------------------------------------*/
/* Definitions of the flags for the Network Interfaces                  */
/*----------------------------------------------------------------------*/
#define IFF_BROADCAST      0x1     /* broadcast flag of the NI */
#define IFF_NOARP          0x2     /* no address resolution protocol */
#define IFF_POINTTOPOINT   0x10    /* interface is point-to-point link */
#define IFF_UP             0x80    /* interface is up */
#define IFF_EXTLOOPBACK    0x4000  /* Loopback externally */
#define IFF_POLL           0x8000  /* interface is a polling type */
/*----------------------------------------------------------------------*/
/* arpreq structure used in ioctl() call to get ARP entries             */
/*----------------------------------------------------------------------*/
struct arpreq
    {
    struct sockaddr arp_pa;     /* protocol address */
    struct sockaddr arp_ha;     /* hardware address */
    int arp_flags;              /* flags */
    };
#define ATF_INUSE  0x01         /* Entry is in use */
#define ATF_COM    0x02         /* Entry is complete */
#define ATF_PERM   0x04         /* permanent entry */
#define ATF_PUBL   0x08         /* publish entry */

/*---------------------------------------------------------------------*/
/* NI table entry.  Used by pNA configuration table and by add_ni().   */
/*---------------------------------------------------------------------*/
struct ni_init
    {
    int (*entry)();     /* address of NI entry point */
    int ipadd;          /* IP address */
    int mtu;            /* maximum transmission length */
    int hwalen;         /* length of hardware address */
    int flags;          /* intErface flags */
    int subnetaddr;     /* subnet mask */
    int dstipaddr;      /* destination ip address */
    int reserved[1];    /* reserved for future use */
    };

/*---------------------------------------------------------------------*/
/* Routing table entry.  Used by pNA configuration table and by        */
/* chng_route().                                                       */
/*---------------------------------------------------------------------*/
struct route
    {
    unsigned long nwipadd;      /* network address */
    unsigned long gwipadd;      /* gateway IP address */
    unsigned long flags;        /* add to Network/host */
    long reserved[1];   /* reserved for future use */
    };

/*---------------------------------------------------------------------*/
/* Arp table entry.  Used by pNA configuration table.                  */
/*---------------------------------------------------------------------*/
struct arp
    {
    long arp_ipadd;     /* IP address of the NI */
    char *arp_hadd;     /* NI (hardware) address */
    long reserved[2];   /* reserved for future use */
    };

/*---------------------------------------------------------------------*/
/* Host table entry. Used by pNA configuration table                   */
/*---------------------------------------------------------------------*/
struct htentry
    {
    unsigned long ht_ipadd;       /* IP address of the host */
    char *ht_name;                /* Name of the host */
    long reserved[2];             /* reserved for future use */
    };

/*---------------------------------------------------------------------*/
/* Structure passed to pna_init() to configure System sockets, mbufs   */
/* and pages                                                           */
/*---------------------------------------------------------------------*/
struct pna_inittbl
    {
    int nsockets;       /* max. no. of sockets */
    int ndesc;          /* max. no. of descriptors per task */
    int nmbufs;         /* max. no. of mbufs */
    int npages;         /* max. no. of pages */
    };

/***********************************************************************/
/* Network Interface (NI) related constants and structures.            */
/***********************************************************************/
/*---------------------------------------------------------------------*/
/* Function number for NI services                                     */
/*---------------------------------------------------------------------*/
#define NI_INIT        0x01         /* NI init call */
#define NI_GETPKB      0x02         /* NI get buffer call */
#define NI_RETPKB      0x03         /* NI return buffer call */
#define NI_SEND        0x04         /* NI send packet call */
#define NI_BROADCAST   0x05         /* NI broadcast call */
#define NI_POLL        0x06         /* NI poll call */
#define NI_IOCTL       0x07         /* NI ioctl call */

/*---------------------------------------------------------------------*/
/* NI paramter block structure. A pointer to this structure is passed  */
/* to the NI on each NI service call.                                  */
/*---------------------------------------------------------------------*/
union nientry
    {
    struct niinit
        {                           /* NI_INIT */
        long (*ap_addr)();          /* pNA entry to receive packet */
        long if_num;                /* NI interface number */
        long ip_addr;               /* NI interface IP address */
        } niinit;

    struct nigetpkb
        {                           /* NI_GETPBK */
        long count;                 /* size of the buffer */
        char *hwa_ptr;              /* Ptr to dest hardware address */
        long if_num;                /* NI interface number */
        } nigetpkb;

    struct niretpkb
        {                           /* NI_RETPBK */
        char *buff_addr;            /* Address of the buffer */
        long if_num;                /* NI interface number */
        } niretpkb;

    struct nisend
        {                           /* NI_SEND */
        char *hwa_ptr;              /* Ptr to dest hardware address */
        char *buff_addr;            /* Address of the packet buffer */
        long count;                 /* Size of the packet */
        long type;                  /* Type of the packet (IP/ARP) */
        long if_num;                /* NI interface number */
        } nisend;

    struct nibrdcast
        {                           /* NI_BRDCAST */
        char *buff_addr;            /* Address of the packet buffer */
        long count;                 /* Size of the packet */
        long type;                  /* Type of the packet (ARP/IP) */
        long if_num;                /* NI interface number */
        } nibrdcast;

    struct nipoll
        {
        long if_num;                /* NI interface number */
        } nipoll;

    struct niioctl
        {
        long cmd;                   /* ioctl command */
        long *arg;                  /* Pointer to ioctl argument */
        long if_num;
        } niioctl;
    };


#endif /* ETHSOCK_H_ */

