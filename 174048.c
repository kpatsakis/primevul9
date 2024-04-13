relpTcpSetPermittedPeers(relpTcp_t __attribute__((unused)) *pThis,
	relpPermittedPeers_t __attribute__((unused)) *pPeers)
{
	ENTER_RELPFUNC;
#ifdef ENABLE_TLS
	int i;
	relpTcpFreePermittedPeers(pThis);
	if(pPeers->nmemb != 0) {
		if((pThis->permittedPeers.peer =
			malloc(sizeof(tcpPermittedPeerEntry_t) * pPeers->nmemb)) == NULL) {
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
		}
		for(i = 0 ; i < pPeers->nmemb ; ++i) {
			if((pThis->permittedPeers.peer[i].name = strdup(pPeers->name[i])) == NULL) {
				ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
			}
			pThis->permittedPeers.peer[i].wildcardRoot = NULL;
			pThis->permittedPeers.peer[i].wildcardLast = NULL;
			CHKRet(relpTcpPermittedPeerWildcardCompile(&(pThis->permittedPeers.peer[i])));
		}
	}
	pThis->permittedPeers.nmemb = pPeers->nmemb;
#else
	ABORT_FINALIZE(RELP_RET_ERR_NO_TLS);
#endif /* #ifdef ENABLE_TLS */
finalize_it:
	LEAVE_RELPFUNC;
}