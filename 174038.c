chkGnutlsCode(relpTcp_t *pThis, char *emsg, relpRetVal ecode, int gnuRet)
{
	char msgbuf[4096];
	int r;

	if(gnuRet == GNUTLS_E_SUCCESS) {
		r = 0;
	} else {
		r = 1;
		snprintf(msgbuf, sizeof(msgbuf), "%s [gnutls error %d: %s]",
			 emsg, gnuRet, gnutls_strerror(gnuRet));
		msgbuf[sizeof(msgbuf)-1] = '\0';
		callOnErr(pThis, msgbuf, ecode);
	}
	return r;
}