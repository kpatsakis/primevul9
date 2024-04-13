EnableKeepAlive(const relpTcp_t *__restrict__ const pThis,
	const relpSrv_t *__restrict__ const pSrv,
	const int sock)
{
	int ret;
	int optval;
	socklen_t optlen;

	optval = 1;
	optlen = sizeof(optval);
	ret = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
	if(ret < 0) {
		pThis->pEngine->dbgprint("librelp: EnableKeepAlive socket call "
					"returns error %d\n", ret);
		goto done;
	}

#	if defined(TCP_KEEPCNT)
	if(pSrv->iKeepAliveProbes > 0) {
		optval = pSrv->iKeepAliveProbes;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPCNT, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		callOnErr(pThis, "librelp cannot set keepalive probes - ignored",
			  RELP_RET_WRN_NO_KEEPALIVE);
	}

#	if defined(TCP_KEEPCNT)
	if(pSrv->iKeepAliveTime > 0) {
		optval = pSrv->iKeepAliveTime;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		callOnErr(pThis, "librelp cannot set keepalive time - ignored",
			  RELP_RET_WRN_NO_KEEPALIVE);
	}

#	if defined(TCP_KEEPCNT)
	if(pSrv->iKeepAliveIntvl > 0) {
		optval = pSrv->iKeepAliveIntvl;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		callOnErr(pThis, "librelp cannot set keepalive intvl - ignored",
			  RELP_RET_WRN_NO_KEEPALIVE);
	}

	// pThis->pEngine->dbgprint("KEEPALIVE enabled for socket %d\n", sock);

done:
	return;
}