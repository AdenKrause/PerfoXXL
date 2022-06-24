(********************************************************************
 * COPYRIGHT (C) BERNECKER + RAINER, AUSTRIA, A-5142 EGGELSBERG
 ********************************************************************
 * Library: EthSock
 * File: EthSock.fun
 * Created: 11.11.2003
 ********************************************************************
 * Functions and function blocks of library EthSock
 ********************************************************************)
FUNCTION fd_zero : UINT
	VAR_INPUT
		p	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION fd_isset : UINT
	VAR_INPUT
		n	:DINT;
		p	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION fd_clr : UINT
	VAR_INPUT
		n	:DINT;
		p	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION fd_set : UINT
	VAR_INPUT
		n	:DINT;
		p	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION socket : DINT
	VAR_INPUT
		domain	:DINT;
		type	:DINT;
		protocol	:DINT;
	END_VAR
END_FUNCTION
FUNCTION accept : DINT
	VAR_INPUT
		s	:DINT;
		addr	:UDINT;
		addrlen	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION bind : DINT
	VAR_INPUT
		s	:DINT;
		name	:UDINT;
		namelen	:DINT;
	END_VAR
END_FUNCTION
FUNCTION connect : DINT
	VAR_INPUT
		s	:DINT;
		name	:UDINT;
		namelen	:DINT;
	END_VAR
END_FUNCTION
FUNCTION getsockopt : DINT
	VAR_INPUT
		s	:DINT;
		level	:DINT;
		optname	:DINT;
		optval	:UDINT;
		optlen	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION setsockopt : DINT
	VAR_INPUT
		s	:DINT;
		level	:DINT;
		optname	:DINT;
		optval	:UDINT;
		optlen	:DINT;
	END_VAR
END_FUNCTION
FUNCTION getsockname : DINT
	VAR_INPUT
		s	:DINT;
		name	:UDINT;
		namelen	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION getpeername : DINT
	VAR_INPUT
		s	:DINT;
		name	:UDINT;
		namelen	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION listen : DINT
	VAR_INPUT
		s	:DINT;
		backlog	:DINT;
	END_VAR
END_FUNCTION
FUNCTION recv : DINT
	VAR_INPUT
		s	:DINT;
		buf	:UDINT;
		buflen	:DINT;
		flags	:DINT;
	END_VAR
END_FUNCTION
FUNCTION recvfrom : DINT
	VAR_INPUT
		s	:DINT;
		buf	:UDINT;
		buflen	:DINT;
		flags	:DINT;
		from	:UDINT;
		fromlen	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION send : DINT
	VAR_INPUT
		s	:DINT;
		buf	:UDINT;
		buflen	:DINT;
		flags	:DINT;
	END_VAR
END_FUNCTION
FUNCTION sendto : DINT
	VAR_INPUT
		s	:DINT;
		buf	:UDINT;
		buflen	:DINT;
		flags	:DINT;
		to	:UDINT;
		tolen	:DINT;
	END_VAR
END_FUNCTION
FUNCTION shutdown : DINT
	VAR_INPUT
		s	:DINT;
		how	:DINT;
	END_VAR
END_FUNCTION
FUNCTION ioctl : DINT
	VAR_INPUT
		s	:DINT;
		cmd	:DINT;
		arg	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION close : DINT
	VAR_INPUT
		s	:DINT;
	END_VAR
END_FUNCTION
FUNCTION sock_select : DINT
	VAR_INPUT
		width	:DINT;
		pReadFds	:UDINT;
		pWriteFds	:UDINT;
		pExceptFds	:UDINT;
		pTimeOut	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION EthGetError : UINT
END_FUNCTION
FUNCTION inet_addr : UDINT
	VAR_INPUT
		pString	:UDINT;
	END_VAR
END_FUNCTION
FUNCTION inet_ntoa_b : UINT
	VAR_INPUT
		ipaddr	:UDINT;
		pString	:UDINT;
	END_VAR
END_FUNCTION
