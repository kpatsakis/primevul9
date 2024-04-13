relpTcpChkPeerFingerprint(relpTcp_t *pThis, gnutls_x509_crt_t cert)
{
	int r = 0;
	int i;
	char fingerprint[20];
	char fpPrintable[512];
	size_t size;
	int8_t found;

	/* obtain the SHA1 fingerprint */
	size = sizeof(fingerprint);
	r = gnutls_x509_crt_get_fingerprint(cert, GNUTLS_DIG_SHA1, fingerprint, &size);
	if(chkGnutlsCode(pThis, "Failed to obtain fingerprint from certificate", RELP_RET_ERR_TLS, r)) {
		r = GNUTLS_E_CERTIFICATE_ERROR; goto done;
	}
	GenFingerprintStr(fingerprint, (int) size, fpPrintable);
	pThis->pEngine->dbgprint("DDDD: peer's certificate SHA1 fingerprint: %s\n", fpPrintable);

	/* now search through the permitted peers to see if we can find a permitted one */
	found = 0;
	pThis->pEngine->dbgprint("DDDD: n peers %d\n", pThis->permittedPeers.nmemb);
	for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i) {
	pThis->pEngine->dbgprint("DDDD: checking peer '%s','%s'\n", fpPrintable, pThis->permittedPeers.peer[i].name);
		if(!strcmp(fpPrintable, pThis->permittedPeers.peer[i].name)) {
			found = 1;
			break;
		}
	}
	if(!found) {
		r = GNUTLS_E_CERTIFICATE_ERROR; goto done;
	}
done:
	if(r != 0) {
		callOnAuthErr(pThis, fpPrintable, "non-permited fingerprint", RELP_RET_AUTH_ERR_FP);
	}
	return r;
}