relpTcpRcv(relpTcp_t *pThis, relpOctet_t *pRcvBuf, ssize_t *pLenBuf)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

#ifdef ENABLE_TLS
	int r;
	if(pThis->bEnableTLS) {
		r = gnutls_record_recv(pThis->session, pRcvBuf, *pLenBuf);
		if(r == GNUTLS_E_INTERRUPTED || r == GNUTLS_E_AGAIN) {
			pThis->pEngine->dbgprint("librelp: gnutls_record_recv must be retried\n");
			pThis->rtryOp = relpTCP_RETRY_recv;
		} else {
			if(r < 0)
				chkGnutlsCode(pThis, "TLS record reception failed", RELP_RET_IO_ERR, r);
			pThis->rtryOp = relpTCP_RETRY_none;
		}
		*pLenBuf = (r < 0) ? -1 : r;
	} else {
#endif /* #ifdef ENABLE_TLS */
		*pLenBuf = recv(pThis->sock, pRcvBuf, *pLenBuf, MSG_DONTWAIT);
		pThis->pEngine->dbgprint("relpTcpRcv: read %zd bytes from sock %d\n",
			*pLenBuf, pThis->sock);
#ifdef ENABLE_TLS
	}
#endif /* #ifdef ENABLE_TLS */

	LEAVE_RELPFUNC;
}