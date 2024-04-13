relpTcpGetCN(relpTcp_t *pThis, gnutls_x509_crt_t cert, char *namebuf, int lenNamebuf)
{
	int r;
	int gnuRet;
	int i,j;
	int bFound;
	size_t size;
	char szDN[1024]; /* this should really be large enough for any non-malicious case... */

	size = sizeof(szDN);
	gnuRet = gnutls_x509_crt_get_dn(cert, (char*)szDN, &size);
	if(chkGnutlsCode(pThis, "Failed to obtain DN from certificate", RELP_RET_ERR_TLS, gnuRet)) {
		r = 1; goto done;
	}

	/* now search for the CN part */
	i = 0;
	bFound = 0;
	while(!bFound && szDN[i] != '\0') {
		/* note that we do not overrun our string due to boolean shortcut
		 * operations. If we have '\0', the if does not match and evaluation
		 * stops. Order of checks is obviously important!
		 */
		if(szDN[i] == 'C' && szDN[i+1] == 'N' && szDN[i+2] == '=') {
			bFound = 1;
			i += 2;
		}
		i++;

	}

	if(!bFound) {
		r = 1; goto done;
	}

	/* we found a common name, now extract it */
	j = 0;
	while(szDN[i] != '\0' && szDN[i] != ',' && j < lenNamebuf-1) {
		if(szDN[i] == '\\') {
			/* hex escapes are not implemented */
			r = 2; goto done;
		} else {
			namebuf[j++] = szDN[i];
		}
		++i; /* char processed */
	}
	namebuf[j] = '\0';

	/* we got it - we ignore the rest of the DN string (if any). So we may
	 * not detect if it contains more than one CN
	 */
	r = 0;

done:
	return r;
}