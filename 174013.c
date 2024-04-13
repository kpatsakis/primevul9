relpTcpSetPrivKey(relpTcp_t *pThis, char *cert)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	
	free(pThis->privKeyFile);
	if(cert == NULL) {
		pThis->privKeyFile = NULL;
	} else {
#		ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION
			if((pThis->privKeyFile = strdup(cert)) == NULL)
				ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
#		else
			ABORT_FINALIZE(RELP_RET_ERR_NO_TLS_AUTH);
#		endif
	}
finalize_it:
	LEAVE_RELPFUNC;
}