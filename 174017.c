relpTcpConstruct(relpTcp_t **ppThis, relpEngine_t *pEngine, int connType, void *pParent)
{
	relpTcp_t *pThis;

	ENTER_RELPFUNC;
	assert(ppThis != NULL);
	if((pThis = calloc(1, sizeof(relpTcp_t))) == NULL) {
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	RELP_CORE_CONSTRUCTOR(pThis, Tcp);
	if(connType == RELP_SRV_CONN) {
		pThis->pSrv = (relpSrv_t*) pParent;
	} else {
		pThis->pClt = (relpClt_t*) pParent;
	}
	pThis->sock = -1;
	pThis->pEngine = pEngine;
	pThis->iSessMax = 500;	/* default max nbr of sessions - TODO: make configurable -- rgerhards, 2008-03-17*/
	pThis->bTLSActive = 0;
	pThis->dhBits = DEFAULT_DH_BITS;
	pThis->pristring = NULL;
	pThis->authmode = eRelpAuthMode_None;
	pThis->caCertFile = NULL;
	pThis->ownCertFile = NULL;
	pThis->privKeyFile = NULL;
	pThis->pUsr = NULL;
	pThis->permittedPeers.nmemb = 0;

	*ppThis = pThis;

finalize_it:
	LEAVE_RELPFUNC;
}