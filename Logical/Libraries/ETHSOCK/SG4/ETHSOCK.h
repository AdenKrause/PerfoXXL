#ifdef __cplusplus
extern "C" {
#endif

/* Automation Studio Generated Header File, Format Version 1.00 */
/* do not change */
#ifndef ETHSOCK_H_
#define ETHSOCK_H_

#include <bur/plctypes.h>

#ifndef _IEC_CONST
#define _IEC_CONST _WEAK const
#endif

/* Constants */
#ifdef _REPLACE_CONST
 #define ETH_AF_NS 6
 #define ERR_ETH_IO 27105U
 #define ERR_SOCKET (-1)
 #define ETH_AF_DLI 13
 #define ETH_AF_ISO 7
 #define ETH_AF_LAT 14
 #define ETH_AF_OSI 7
 #define ETH_AF_PUP 4
 #define ETH_AF_SNA 11
 #define ERR_ETH_DOM 27137U
 #define ERR_ETH_MAX 27181U
 #define ETH_AF_ECMA 8
 #define ETH_AF_INET 2
 #define ETH_AF_UNIX 1
 #define ETH_FIONBIO 16
 #define ETH_SO_TYPE 4104
 #define ERR_ETH_2BIG 27107U
 #define ERR_ETH_BADF 27109U
 #define ERR_ETH_BASE 27100U
 #define ERR_ETH_BUSY 27116U
 #define ERR_ETH_FBIG 27127U
 #define ERR_ETH_INTR 27104U
 #define ERR_ETH_LOOP 27164U
 #define ERR_ETH_NOSR 27174U
 #define ERR_ETH_NXIO 27106U
 #define ERR_ETH_PERM 27101U
 #define ERR_ETH_PIPE 27132U
 #define ERR_ETH_ROFS 27130U
 #define ERR_ETH_SRCH 27103U
 #define ERR_ETH_TIME 27179U
 #define ERR_ETH_XDEV 27118U
 #define ETH_AF_CCITT 10
 #define ETH_AF_CHAOS 5
 #define ETH_FIONREAD 1
 #define ETH_SOCK_RAW 3
 #define ETH_SOCK_RDM 4
 #define ETH_SO_DEBUG 1
 #define ETH_SO_ERROR 4103
 #define ERR_ETH_AGAIN 27111U
 #define ERR_ETH_CHILD 27110U
 #define ERR_ETH_EXIST 27117U
 #define ERR_ETH_FAULT 27114U
 #define ERR_ETH_INVAL 27122U
 #define ERR_ETH_ISDIR 27121U
 #define ERR_ETH_MFILE 27124U
 #define ERR_ETH_MLINK 27131U
 #define ERR_ETH_NFILE 27123U
 #define ERR_ETH_NODEV 27119U
 #define ERR_ETH_NOENT 27102U
 #define ERR_ETH_NOLCK 27134U
 #define ERR_ETH_NOMEM 27112U
 #define ERR_ETH_NOMGR 27248U
 #define ERR_ETH_NOMSG 27180U
 #define ERR_ETH_NOSPC 27128U
 #define ERR_ETH_NOSTR 27175U
 #define ERR_ETH_NOSYS 27171U
 #define ERR_ETH_NOTTY 27125U
 #define ERR_ETH_PROTO 27176U
 #define ERR_ETH_RANGE 27138U
 #define ERR_ETH_SPIPE 27129U
 #define ETH_AF_DECNET 12
 #define ETH_AF_HYLINK 15
 #define ETH_AF_UNSPEC 0
 #define ETH_SOMAXCONN 5
 #define ETH_SO_LINGER 128
 #define ETH_SO_RCVBUF 4098
 #define ETH_SO_SNDBUF 4097
 #define ERR_ETH_ACCESS 27113U
 #define ERR_ETH_BADMSG 27177U
 #define ERR_ETH_DEADLK 27133U
 #define ERR_ETH_ISCONN 27156U
 #define ERR_ETH_NOBUFS 27155U
 #define ERR_ETH_NODATA 27178U
 #define ERR_ETH_NOEXEC 27108U
 #define ERR_ETH_NOTBLK 27166U
 #define ERR_ETH_NOTDIR 27120U
 #define ERR_ETH_NOTSUP 27135U
 #define ERR_ETH_TXTBSY 27163U
 #define ETH_AF_DATAKIT 9
 #define ETH_AF_IMPLINK 3
 #define ETH_FD_SETSIZE 256
 #define ETH_INADDR_ANY 0
 #define ETH_IPPROTO_IP 0
 #define ETH_SOCK_DGRAM 2
 #define ETH_SOL_SOCKET 65535
 #define ERR_ETH_ALREADY 27169U
 #define ERR_ETH_MSGSIZE 27136U
 #define ERR_ETH_NETDOWN 27162U
 #define ERR_ETH_NOTCONN 27157U
 #define ERR_ETH_NOTSOCK 27150U
 #define ERR_ETH_UNKNOWN 27249U
 #define ETH_IPPROTO_GGP 3
 #define ETH_IPPROTO_IDP 17
 #define ETH_IPPROTO_MAX 256
 #define ETH_IPPROTO_PUP 12
 #define ETH_IPPROTO_RAW 255
 #define ETH_IPPROTO_TCP 6
 #define ETH_IPPROTO_UDP 17
 #define ETH_SOCK_STREAM 1
 #define ETH_SO_RCVLOWAT 4100
 #define ETH_SO_RCVTIMEO 4102
 #define ETH_SO_SNDLOWAT 4099
 #define ETH_SO_SNDTIMEO 4101
 #define ERR_ETH_AFNOTSUP 27147U
 #define ERR_ETH_CANCELED 27172U
 #define ERR_ETH_HOSTDOWN 27167U
 #define ERR_ETH_NETRESET 27152U
 #define ERR_ETH_NOTEMPTY 27115U
 #define ERR_ETH_OPNOTSUP 27145U
 #define ERR_ETH_PFNOTSUP 27146U
 #define ERR_ETH_SHUTDOWN 27158U
 #define ERR_ETH_TIMEDOUT 27160U
 #define ETH_AF_APPLETALK 16
 #define ETH_IPPROTO_ICMP 1
 #define ETH_IPPROTO_IGMP 2
 #define ETH_SO_BROADCAST 32
 #define ETH_SO_DONTROUTE 16
 #define ETH_SO_KEEPALIVE 8
 #define ETH_SO_OOBINLINE 256
 #define ETH_SO_REUSEADDR 4
 #define ERR_ETH_ADDRINUSE 27148U
 #define ERR_ETH_CONNRESET 27154U
 #define ERR_ETH_PROTOTYPE 27141U
 #define ETH_INET_ADDR_LEN 18U
 #define ETH_SO_ACCEPTCONN 2
 #define ERR_ETH_FD_SETSIZE 27200U
 #define ERR_ETH_INPROGRESS 27168U
 #define ERR_ETH_NETUNREACH 27151U
 #define ERR_ETH_NOPROTOOPT 27142U
 #define ERR_ETH_SOCKNOTSUP 27144U
 #define ERR_ETH_WOULDBLOCK 27170U
 #define ETH_SOCK_SEQPACKET 5
 #define ERR_ETH_CONNABORTED 27153U
 #define ERR_ETH_CONNREFUSED 27161U
 #define ERR_ETH_DESTADDRREQ 27140U
 #define ERR_ETH_HOSTUNREACH 27165U
 #define ERR_ETH_NAMETOOLONG 27126U
 #define ERR_ETH_PROTONOTSUP 27143U
 #define ERR_ETH_TOOMANYREFS 27159U
 #define ERR_ETH_ADDRNOTAVAIL 27149U
#else
 _IEC_CONST signed long ETH_AF_NS = 6;
 _IEC_CONST unsigned short ERR_ETH_IO = 27105U;
 _IEC_CONST signed long ERR_SOCKET = -1;
 _IEC_CONST signed long ETH_AF_DLI = 13;
 _IEC_CONST signed long ETH_AF_ISO = 7;
 _IEC_CONST signed long ETH_AF_LAT = 14;
 _IEC_CONST signed long ETH_AF_OSI = 7;
 _IEC_CONST signed long ETH_AF_PUP = 4;
 _IEC_CONST signed long ETH_AF_SNA = 11;
 _IEC_CONST unsigned short ERR_ETH_DOM = 27137U;
 _IEC_CONST unsigned short ERR_ETH_MAX = 27181U;
 _IEC_CONST signed long ETH_AF_ECMA = 8;
 _IEC_CONST signed long ETH_AF_INET = 2;
 _IEC_CONST signed long ETH_AF_UNIX = 1;
 _IEC_CONST signed long ETH_FIONBIO = 16;
 _IEC_CONST signed long ETH_SO_TYPE = 4104;
 _IEC_CONST unsigned short ERR_ETH_2BIG = 27107U;
 _IEC_CONST unsigned short ERR_ETH_BADF = 27109U;
 _IEC_CONST unsigned short ERR_ETH_BASE = 27100U;
 _IEC_CONST unsigned short ERR_ETH_BUSY = 27116U;
 _IEC_CONST unsigned short ERR_ETH_FBIG = 27127U;
 _IEC_CONST unsigned short ERR_ETH_INTR = 27104U;
 _IEC_CONST unsigned short ERR_ETH_LOOP = 27164U;
 _IEC_CONST unsigned short ERR_ETH_NOSR = 27174U;
 _IEC_CONST unsigned short ERR_ETH_NXIO = 27106U;
 _IEC_CONST unsigned short ERR_ETH_PERM = 27101U;
 _IEC_CONST unsigned short ERR_ETH_PIPE = 27132U;
 _IEC_CONST unsigned short ERR_ETH_ROFS = 27130U;
 _IEC_CONST unsigned short ERR_ETH_SRCH = 27103U;
 _IEC_CONST unsigned short ERR_ETH_TIME = 27179U;
 _IEC_CONST unsigned short ERR_ETH_XDEV = 27118U;
 _IEC_CONST signed long ETH_AF_CCITT = 10;
 _IEC_CONST signed long ETH_AF_CHAOS = 5;
 _IEC_CONST signed long ETH_FIONREAD = 1;
 _IEC_CONST signed long ETH_SOCK_RAW = 3;
 _IEC_CONST signed long ETH_SOCK_RDM = 4;
 _IEC_CONST signed long ETH_SO_DEBUG = 1;
 _IEC_CONST signed long ETH_SO_ERROR = 4103;
 _IEC_CONST unsigned short ERR_ETH_AGAIN = 27111U;
 _IEC_CONST unsigned short ERR_ETH_CHILD = 27110U;
 _IEC_CONST unsigned short ERR_ETH_EXIST = 27117U;
 _IEC_CONST unsigned short ERR_ETH_FAULT = 27114U;
 _IEC_CONST unsigned short ERR_ETH_INVAL = 27122U;
 _IEC_CONST unsigned short ERR_ETH_ISDIR = 27121U;
 _IEC_CONST unsigned short ERR_ETH_MFILE = 27124U;
 _IEC_CONST unsigned short ERR_ETH_MLINK = 27131U;
 _IEC_CONST unsigned short ERR_ETH_NFILE = 27123U;
 _IEC_CONST unsigned short ERR_ETH_NODEV = 27119U;
 _IEC_CONST unsigned short ERR_ETH_NOENT = 27102U;
 _IEC_CONST unsigned short ERR_ETH_NOLCK = 27134U;
 _IEC_CONST unsigned short ERR_ETH_NOMEM = 27112U;
 _IEC_CONST unsigned short ERR_ETH_NOMGR = 27248U;
 _IEC_CONST unsigned short ERR_ETH_NOMSG = 27180U;
 _IEC_CONST unsigned short ERR_ETH_NOSPC = 27128U;
 _IEC_CONST unsigned short ERR_ETH_NOSTR = 27175U;
 _IEC_CONST unsigned short ERR_ETH_NOSYS = 27171U;
 _IEC_CONST unsigned short ERR_ETH_NOTTY = 27125U;
 _IEC_CONST unsigned short ERR_ETH_PROTO = 27176U;
 _IEC_CONST unsigned short ERR_ETH_RANGE = 27138U;
 _IEC_CONST unsigned short ERR_ETH_SPIPE = 27129U;
 _IEC_CONST signed long ETH_AF_DECNET = 12;
 _IEC_CONST signed long ETH_AF_HYLINK = 15;
 _IEC_CONST signed long ETH_AF_UNSPEC = 0;
 _IEC_CONST signed long ETH_SOMAXCONN = 5;
 _IEC_CONST signed long ETH_SO_LINGER = 128;
 _IEC_CONST signed long ETH_SO_RCVBUF = 4098;
 _IEC_CONST signed long ETH_SO_SNDBUF = 4097;
 _IEC_CONST unsigned short ERR_ETH_ACCESS = 27113U;
 _IEC_CONST unsigned short ERR_ETH_BADMSG = 27177U;
 _IEC_CONST unsigned short ERR_ETH_DEADLK = 27133U;
 _IEC_CONST unsigned short ERR_ETH_ISCONN = 27156U;
 _IEC_CONST unsigned short ERR_ETH_NOBUFS = 27155U;
 _IEC_CONST unsigned short ERR_ETH_NODATA = 27178U;
 _IEC_CONST unsigned short ERR_ETH_NOEXEC = 27108U;
 _IEC_CONST unsigned short ERR_ETH_NOTBLK = 27166U;
 _IEC_CONST unsigned short ERR_ETH_NOTDIR = 27120U;
 _IEC_CONST unsigned short ERR_ETH_NOTSUP = 27135U;
 _IEC_CONST unsigned short ERR_ETH_TXTBSY = 27163U;
 _IEC_CONST signed long ETH_AF_DATAKIT = 9;
 _IEC_CONST signed long ETH_AF_IMPLINK = 3;
 _IEC_CONST signed long ETH_FD_SETSIZE = 256;
 _IEC_CONST signed long ETH_INADDR_ANY = 0;
 _IEC_CONST signed long ETH_IPPROTO_IP = 0;
 _IEC_CONST signed long ETH_SOCK_DGRAM = 2;
 _IEC_CONST signed long ETH_SOL_SOCKET = 65535;
 _IEC_CONST unsigned short ERR_ETH_ALREADY = 27169U;
 _IEC_CONST unsigned short ERR_ETH_MSGSIZE = 27136U;
 _IEC_CONST unsigned short ERR_ETH_NETDOWN = 27162U;
 _IEC_CONST unsigned short ERR_ETH_NOTCONN = 27157U;
 _IEC_CONST unsigned short ERR_ETH_NOTSOCK = 27150U;
 _IEC_CONST unsigned short ERR_ETH_UNKNOWN = 27249U;
 _IEC_CONST signed long ETH_IPPROTO_GGP = 3;
 _IEC_CONST signed long ETH_IPPROTO_IDP = 17;
 _IEC_CONST signed long ETH_IPPROTO_MAX = 256;
 _IEC_CONST signed long ETH_IPPROTO_PUP = 12;
 _IEC_CONST signed long ETH_IPPROTO_RAW = 255;
 _IEC_CONST signed long ETH_IPPROTO_TCP = 6;
 _IEC_CONST signed long ETH_IPPROTO_UDP = 17;
 _IEC_CONST signed long ETH_SOCK_STREAM = 1;
 _IEC_CONST signed long ETH_SO_RCVLOWAT = 4100;
 _IEC_CONST signed long ETH_SO_RCVTIMEO = 4102;
 _IEC_CONST signed long ETH_SO_SNDLOWAT = 4099;
 _IEC_CONST signed long ETH_SO_SNDTIMEO = 4101;
 _IEC_CONST unsigned short ERR_ETH_AFNOTSUP = 27147U;
 _IEC_CONST unsigned short ERR_ETH_CANCELED = 27172U;
 _IEC_CONST unsigned short ERR_ETH_HOSTDOWN = 27167U;
 _IEC_CONST unsigned short ERR_ETH_NETRESET = 27152U;
 _IEC_CONST unsigned short ERR_ETH_NOTEMPTY = 27115U;
 _IEC_CONST unsigned short ERR_ETH_OPNOTSUP = 27145U;
 _IEC_CONST unsigned short ERR_ETH_PFNOTSUP = 27146U;
 _IEC_CONST unsigned short ERR_ETH_SHUTDOWN = 27158U;
 _IEC_CONST unsigned short ERR_ETH_TIMEDOUT = 27160U;
 _IEC_CONST signed long ETH_AF_APPLETALK = 16;
 _IEC_CONST signed long ETH_IPPROTO_ICMP = 1;
 _IEC_CONST signed long ETH_IPPROTO_IGMP = 2;
 _IEC_CONST signed long ETH_SO_BROADCAST = 32;
 _IEC_CONST signed long ETH_SO_DONTROUTE = 16;
 _IEC_CONST signed long ETH_SO_KEEPALIVE = 8;
 _IEC_CONST signed long ETH_SO_OOBINLINE = 256;
 _IEC_CONST signed long ETH_SO_REUSEADDR = 4;
 _IEC_CONST unsigned short ERR_ETH_ADDRINUSE = 27148U;
 _IEC_CONST unsigned short ERR_ETH_CONNRESET = 27154U;
 _IEC_CONST unsigned short ERR_ETH_PROTOTYPE = 27141U;
 _IEC_CONST unsigned char ETH_INET_ADDR_LEN = 18U;
 _IEC_CONST signed long ETH_SO_ACCEPTCONN = 2;
 _IEC_CONST unsigned short ERR_ETH_FD_SETSIZE = 27200U;
 _IEC_CONST unsigned short ERR_ETH_INPROGRESS = 27168U;
 _IEC_CONST unsigned short ERR_ETH_NETUNREACH = 27151U;
 _IEC_CONST unsigned short ERR_ETH_NOPROTOOPT = 27142U;
 _IEC_CONST unsigned short ERR_ETH_SOCKNOTSUP = 27144U;
 _IEC_CONST unsigned short ERR_ETH_WOULDBLOCK = 27170U;
 _IEC_CONST signed long ETH_SOCK_SEQPACKET = 5;
 _IEC_CONST unsigned short ERR_ETH_CONNABORTED = 27153U;
 _IEC_CONST unsigned short ERR_ETH_CONNREFUSED = 27161U;
 _IEC_CONST unsigned short ERR_ETH_DESTADDRREQ = 27140U;
 _IEC_CONST unsigned short ERR_ETH_HOSTUNREACH = 27165U;
 _IEC_CONST unsigned short ERR_ETH_NAMETOOLONG = 27126U;
 _IEC_CONST unsigned short ERR_ETH_PROTONOTSUP = 27143U;
 _IEC_CONST unsigned short ERR_ETH_TOOMANYREFS = 27159U;
 _IEC_CONST unsigned short ERR_ETH_ADDRNOTAVAIL = 27149U;
#endif


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
unsigned short inet_ntoa_b(unsigned long ipaddr, unsigned long pString);
unsigned long inet_addr(unsigned long pString);
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


/* structure and symbol definitions only for i386-Targets (VxWorks) */


/*
 * An option specification consists of an opthdr, followed by the value of
 * the option.  An options buffer contains one or more options.  The len
 * field of opthdr specifies the length of the option value in bytes.  This
 * length must be a multiple of sizeof (long) (use OPTLEN macro).
 */

struct opthdr {
	long 	level;			/* protocol level affected */
	long	name;			/* option to modify */
	long	len;			/* length of option value */
};

#define OPTLEN(x) ((((x) + sizeof (long) - 1) / sizeof (long)) * sizeof (long))
#define OPTVAL(opt) ((char *)(opt + 1))

/*
 * Structure used for manipulating linger option.
 */
struct	linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time */
};

#define	MSG_OOB			0x1			/* process out-of-band data */
#define	MSG_PEEK		0x2			/* peek at incoming message */
#define	MSG_DONTROUTE	0x4			/* send without using routing tables */
#define MSG_NO_UIO		0x8			/* for system use only */
#define	MSG_EOR			0x8			/* data completes record */
#define	MSG_TRUNC		0x10		/* data discarded before delivery */
#define	MSG_CTRUNC		0x20		/* control data lost before delivery */
#define	MSG_WAITALL		0x40		/* wait for full request or error */
#define	MSG_DONTWAIT	0x80		/* this message should be nonblocking */
#define MSG_MBUF		0x10000		/* mbuf interface - WRS ext. */
#define MSG_IGNORE_SB	0x20000		/* ignore socket write buf - WRS ext. */

#define	MSG_MAXIOVLEN	16

#define	NBBY	8		/* number of bits in a byte */
typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */


/* socket i/o controls */
/*
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#define	IOCPARM_MASK	0x7f		/* parameters must be < 128 bytes */
#define	IOC_VOID	0x20000000	/* no parameters */
#define	IOC_OUT		0x40000000	/* copy out parameters */
#define	IOC_IN		0x80000000	/* copy in parameters */
#define	IOC_INOUT	(IOC_IN|IOC_OUT)

/* had to change macros from 'x' to (x) and had to pass in litterals as 'i'
 * rather than i, because ANSIc doesn't behave in the same manner as
 * the traditional c compilers
 */
#define	_IO(x,y)		(IOC_VOID|((x)<<8)|y)
#define	_IOR(x,y,t)		(IOC_OUT|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|y)
#define	_IOW(x,y,t)		(IOC_IN|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|y)
#define	_IOWR(x,y,t)	(IOC_INOUT|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|y)

#define	SIOCSHIWAT		_IOW('s',  0, int)		/* set high watermark */
#define	SIOCGHIWAT		_IOR('s',  1, int)		/* get high watermark */
#define	SIOCSLOWAT		_IOW('s',  2, int)		/* set low watermark */
#define	SIOCGLOWAT		_IOR('s',  3, int)		/* get low watermark */
#define	SIOCATMARK		_IOR('s',  7, int)		/* at oob mark? */
#define	SIOCSPGRP		_IOW('s',  8, int)		/* set process group */
#define	SIOCGPGRP		_IOR('s',  9, int)		/* get process group */

#define	SIOCADDRT		_IOW('r', 10, struct ortentry)	/* add route */
#define	SIOCDELRT		_IOW('r', 11, struct ortentry)	/* delete route */

#define	SIOCSIFADDR		_IOW('i', 12, struct ifreq)	/* set ifnet address */
#define	OSIOCGIFADDR	_IOWR('i', 13, struct ifreq)	/* get ifnet address */
#define	SIOCGIFADDR		_IOWR('i', 33, struct ifreq)	/* get ifnet address */
#define	SIOCSIFDSTADDR	 _IOW('i', 14, struct ifreq)	/* set p-p address */
#define	OSIOCGIFDSTADDR	_IOWR('i', 15, struct ifreq)	/* get p-p address */
#define	SIOCGIFDSTADDR	_IOWR('i', 34, struct ifreq)	/* get p-p address */
#define	SIOCSIFFLAGS	 _IOW('i', 16, struct ifreq)	/* set ifnet flags */
#define	SIOCGIFFLAGS	_IOWR('i', 17, struct ifreq)	/* get ifnet flags */
#define	OSIOCGIFBRDADDR	_IOWR('i', 18, struct ifreq)	/* get broadcast addr */
#define	SIOCGIFBRDADDR	_IOWR('i', 35, struct ifreq)	/* get broadcast addr */
#define	SIOCSIFBRDADDR	 _IOW('i', 19, struct ifreq)	/* set broadcast addr */
#define	SIOCSARP		_IOW('i', 30, struct arpreq)	/* set arp entry */
#define	SIOCGARP		_IOWR('i',31, struct arpreq)	/* get arp entry */
#define	SIOCDARP		_IOW('i', 32, struct arpreq)	/* delete arp entry */

#define	OSIOCGIFCONF	_IOWR('i', 20, struct ifconf)	/* get ifnet list */
#define	SIOCGIFCONF		_IOWR('i', 36, struct ifconf)	/* get ifnet list */
#define	OSIOCGIFNETMASK	_IOWR('i', 21, struct ifreq)	/* get net addr mask */
#define	SIOCGIFNETMASK	_IOWR('i', 37, struct ifreq)	/* get net addr mask */
#define	SIOCSIFNETMASK	_IOW('i', 22, struct ifreq)	/* set net addr mask */
#define	SIOCGIFMETRIC	_IOWR('i', 23, struct ifreq)	/* get IF metric */
#define	SIOCSIFMETRIC	_IOW('i', 24, struct ifreq)	/* set IF metric */
#define	SIOCDIFADDR		_IOW('i', 25, struct ifreq)	/* delete IF addr */
#define	SIOCAIFADDR		_IOW('i', 26, struct ifaliasreq)/* add/chg IF alias */

#define	SIOCADDMULTI	_IOW('i', 49, struct ifreq)	/* add m'cast addr */
#define	SIOCDELMULTI	_IOW('i', 50, struct ifreq)	/* del m'cast addr */
#define SIOCGETMULTI    _IOWR('i', 51, struct ifmreq)  /* get m'cast addr */


#define	SIOCSIFMTU		_IOW('i', 127, struct ifreq)	/* set ifnet mtu */
#define	SIOCGIFMTU		_IOWR('i', 126, struct ifreq)	/* get ifnet mtu */
#define	SIOCSIFASYNCMAP _IOW('i', 125, struct ifreq)	/* set ppp asyncmap */
#define	SIOCGIFASYNCMAP _IOWR('i', 124, struct ifreq)	/* get ppp asyncmap */


#endif /* ETHSOCK_H_ */

#ifdef __cplusplus
};
#endif

