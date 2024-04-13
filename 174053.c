relpTcpAcceptConnReqInitTLS(relpTcp_t *pThis, relpSrv_t *pSrv)
{
	int r;
	ENTER_RELPFUNC;

	r = gnutls_init(&pThis->session, GNUTLS_SERVER);
	if(chkGnutlsCode(pThis, "Failed to initialize GnuTLS", RELP_RET_ERR_TLS_SETUP, r)) {
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}

	gnutls_session_set_ptr(pThis->session, pThis);

	if(pSrv->pTcp->pristring != NULL)
		pThis->pristring = strdup(pSrv->pTcp->pristring);
	pThis->authmode = pSrv->pTcp->authmode;
	pThis->pUsr = pSrv->pUsr;
	CHKRet(relpTcpTLSSetPrio(pThis));

	if(isAnonAuth(pSrv->pTcp)) {
		r = gnutls_credentials_set(pThis->session, GNUTLS_CRD_ANON, pSrv->pTcp->anoncredSrv);
		if(chkGnutlsCode(pThis, "Failed setting anonymous credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	} else { /* cert-based auth */
		if(pSrv->pTcp->caCertFile == NULL) {
			gnutls_certificate_send_x509_rdn_sequence(pThis->session, 0);
		}
		r = gnutls_credentials_set(pThis->session, GNUTLS_CRD_CERTIFICATE, pSrv->pTcp->xcred);
		if(chkGnutlsCode(pThis, "Failed setting certificate credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	}
	gnutls_dh_set_prime_bits(pThis->session, pThis->dhBits);
	gnutls_certificate_server_set_request(pThis->session, GNUTLS_CERT_REQUEST);

	gnutls_transport_set_ptr(pThis->session, (gnutls_transport_ptr_t) pThis->sock);
	r = gnutls_handshake(pThis->session);
	if(r == GNUTLS_E_INTERRUPTED || r == GNUTLS_E_AGAIN) {
		pThis->pEngine->dbgprint("librelp: gnutls_handshake retry necessary (this is OK and expected)\n");
		pThis->rtryOp = relpTCP_RETRY_handshake;
	} else if(r != GNUTLS_E_SUCCESS) {
		chkGnutlsCode(pThis, "TLS handshake failed", RELP_RET_ERR_TLS_HANDS, r);
		ABORT_FINALIZE(RELP_RET_ERR_TLS_HANDS);
	}

	pThis->bTLSActive = 1;

finalize_it:
	LEAVE_RELPFUNC;
}