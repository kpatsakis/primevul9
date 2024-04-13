relpTcpTLSSetPrio(relpTcp_t *pThis)
{
	int r;
	char pristringBuf[4096];
	char *pristring;
	ENTER_RELPFUNC;
	/* Compute priority string (in simple cases where the user does not care...) */
	if(pThis->pristring == NULL) {
		if(pThis->bEnableTLSZip) {
			strncpy(pristringBuf, "NORMAL:+ANON-DH:+COMP-ALL", sizeof(pristringBuf));
		} else {
			strncpy(pristringBuf, "NORMAL:+ANON-DH:+COMP-NULL", sizeof(pristringBuf));
		}
		pristringBuf[sizeof(pristringBuf)-1] = '\0';
		pristring = pristringBuf;
	} else {
		pristring = pThis->pristring;
	}

	r = gnutls_priority_set_direct(pThis->session, pristring, NULL);
	if(r == GNUTLS_E_INVALID_REQUEST) {
		ABORT_FINALIZE(RELP_RET_INVLD_TLS_PRIO);
	} else if(r != GNUTLS_E_SUCCESS) {
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}
finalize_it:
	if(iRet != RELP_RET_OK)
		chkGnutlsCode(pThis, "Failed to set GnuTLS priority", iRet, r);
	LEAVE_RELPFUNC;
}