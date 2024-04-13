relpTcpSetCACert(relpTcp_t *pThis, char *cert)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	
	free(pThis->caCertFile);
	if(cert == NULL) {
		pThis->caCertFile = NULL;
	} else {
		if((pThis->caCertFile = strdup(cert)) == NULL)
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
finalize_it:
	LEAVE_RELPFUNC;
}