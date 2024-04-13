relpTcpAddToCertNamesBuffer(relpTcp_t *const pThis,
	char *const buf,
	const size_t buflen,
	int *p_currIdx,
	const char *const certName)
{
	int r = 0;
	assert(buf != NULL);
	assert(p_currIdx != NULL);
	const int currIdx = *p_currIdx;
	const int n = snprintf(buf + currIdx, buflen - currIdx,
		"DNSname: %s; ", certName);
	if(n < 0 || n >= (int) (buflen - currIdx)) {
		callOnAuthErr(pThis, "", "certificate validation failed, names "
			"inside certifcate are way to long (> 32KiB)",
			RELP_RET_AUTH_CERT_INVL);
		r = GNUTLS_E_CERTIFICATE_ERROR;
	} else {
		*p_currIdx += n;
	}
	return r;
}