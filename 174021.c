relpTcpConnect(relpTcp_t *pThis, int family, unsigned char *port, unsigned char *host, unsigned char *clientIP)
{
	struct addrinfo *res = NULL;
	struct addrinfo hints;
	struct addrinfo *reslocal = NULL;
	struct pollfd pfd;

	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	assert(port != NULL);
	assert(host != NULL);
	assert(pThis->sock == -1);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo((char*)host, (char*)port, &hints, &res) != 0) {
		pThis->pEngine->dbgprint("error %d in getaddrinfo\n", errno);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}
	
	if((pThis->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	if(clientIP != NULL) {
		if(getaddrinfo((char*)clientIP, (char*)NULL, &hints, &reslocal) != 0) {
			pThis->pEngine->dbgprint("error %d in getaddrinfo of clientIP\n", errno);
			ABORT_FINALIZE(RELP_RET_IO_ERR);
		}
		if(bind(pThis->sock, reslocal->ai_addr, reslocal->ai_addrlen) != 0) {
			ABORT_FINALIZE(RELP_RET_IO_ERR);
		}
	}

	fcntl(pThis->sock, F_SETFL, O_NONBLOCK);
	connect(pThis->sock, res->ai_addr, res->ai_addrlen);

	pfd.fd = pThis->sock;
	pfd.events = POLLOUT;

	if (poll(&pfd, 1, pThis->connTimeout * 1000) != 1) {
		pThis->pEngine->dbgprint("connection timed out after %d seconds\n", pThis->connTimeout);
		ABORT_FINALIZE(RELP_RET_TIMED_OUT);
	}

	int so_error;
	socklen_t len = sizeof so_error;

	getsockopt(pThis->sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
	if (so_error != 0) {
		pThis->pEngine->dbgprint("socket has an error %d\n", so_error);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}


#ifdef ENABLE_TLS
	if(pThis->bEnableTLS) {
		CHKRet(relpTcpConnectTLSInit(pThis));
		pThis->bTLSActive = 1;
	}
#endif /* #ifdef ENABLE_TLS */

finalize_it:
	if(res != NULL)
		freeaddrinfo(res);
	if(reslocal != NULL)
		freeaddrinfo(reslocal);

	if(iRet != RELP_RET_OK) {
		if(pThis->sock != -1) {
			close(pThis->sock);
			pThis->sock = -1;
		}
	}

	LEAVE_RELPFUNC;
}