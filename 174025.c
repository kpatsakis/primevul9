callOnAuthErr(relpTcp_t *pThis, char *authdata, char *emsg, relpRetVal ecode)
{
	pThis->pEngine->dbgprint("librelp: auth error: authdata:'%s', ecode %d, "
		"emsg '%s'\n", authdata, ecode, emsg);
	if(pThis->pEngine->onAuthErr != NULL) {
		pThis->pEngine->onAuthErr(pThis->pUsr, authdata, emsg, ecode);
	}
}