relpTcpSetGnuTLSPriString(relpTcp_t *pThis, char *pristr)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	
	free(pThis->pristring);
	if(pristr == NULL) {
		pThis->pristring = NULL;
	} else {
		if((pThis->pristring = strdup(pristr)) == NULL)
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
finalize_it:
	LEAVE_RELPFUNC;
}