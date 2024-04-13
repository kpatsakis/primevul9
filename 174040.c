relpTcpRtryHandshake(relpTcp_t *pThis)
{
	int r;
	ENTER_RELPFUNC;
	r = gnutls_handshake(pThis->session);
	if(r < 0) {
		pThis->pEngine->dbgprint("librelp: state %d during retry handshake: %s\n", r, gnutls_strerror(r));
	}
	if(r == GNUTLS_E_INTERRUPTED || r == GNUTLS_E_AGAIN) {
		; /* nothing to do, just keep our status... */
	} else if(r == 0) {
		pThis->rtryOp = relpTCP_RETRY_none;
	} else {
		chkGnutlsCode(pThis, "TLS handshake failed", RELP_RET_ERR_TLS_SETUP, r);
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}

finalize_it:
	LEAVE_RELPFUNC;
}