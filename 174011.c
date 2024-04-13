relpTcpLstnInitTLS(relpTcp_t *pThis)
{
	int r;
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	#if GNUTLS_VERSION_NUMBER <= 0x020b00
	/* gcry_control must be called first, so that the thread system is correctly set up */
	gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
	#endif
	gnutls_global_init();
	/* uncomment for (very intense) debug help
	 * gnutls_global_set_log_function(logFunction);
	 * gnutls_global_set_log_level(10); // 0 (no) to 9 (most), 10 everything
	 */

	if(isAnonAuth(pThis)) {
		r = gnutls_dh_params_init(&pThis->dh_params);
		if(chkGnutlsCode(pThis, "Failed to initialize dh_params", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		r = gnutls_dh_params_generate2(pThis->dh_params, pThis->dhBits);
		if(chkGnutlsCode(pThis, "Failed to generate dh_params", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		r = gnutls_anon_allocate_server_credentials(&pThis->anoncredSrv);
		if(chkGnutlsCode(pThis, "Failed to allocate server credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		gnutls_anon_set_server_dh_params(pThis->anoncredSrv, pThis->dh_params);
	} else {
#		ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION
		r = gnutls_certificate_allocate_credentials(&pThis->xcred);
		if(chkGnutlsCode(pThis, "Failed to allocate certificate credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		if(pThis->caCertFile != NULL) {
			r = gnutls_certificate_set_x509_trust_file(pThis->xcred,
				pThis->caCertFile, GNUTLS_X509_FMT_PEM);
			if(r < 0) {
				chkGnutlsCode(pThis, "Failed to set certificate trust files",
								RELP_RET_ERR_TLS_SETUP, r);
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			}
			pThis->pEngine->dbgprint("librelp: obtained %d certificates from %s\n", r, pThis->caCertFile);
		}
		r = gnutls_certificate_set_x509_key_file (pThis->xcred,
			pThis->ownCertFile, pThis->privKeyFile, GNUTLS_X509_FMT_PEM);
		if(chkGnutlsCode(pThis, "Failed to set certificate key files", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		if(pThis->authmode == eRelpAuthMode_None)
			pThis->authmode = eRelpAuthMode_Fingerprint;
		gnutls_certificate_set_verify_function(pThis->xcred, relpTcpVerifyCertificateCallback);
#		else /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION   */
		ABORT_FINALIZE(RELP_RET_ERR_NO_TLS_AUTH);
#		endif /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION   */
	}
finalize_it:
	LEAVE_RELPFUNC;
}