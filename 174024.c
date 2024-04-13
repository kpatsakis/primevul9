relpTcpSetOwnCert(relpTcp_t *pThis, char *cert)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	
	free(pThis->ownCertFile);
	if(cert == NULL) {
		pThis->ownCertFile = NULL;
	} else {
		if((pThis->ownCertFile = strdup(cert)) == NULL)
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
finalize_it:
	LEAVE_RELPFUNC;
}