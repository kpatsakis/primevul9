relpTcpChkOnePeerName(relpTcp_t *pThis, char *peername, int *pbFoundPositiveMatch)
{
	int i;

	for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i) {
		if(pThis->permittedPeers.peer[i].wildcardRoot == NULL) {
			/* simple string, only, no wildcards */
			if(!strcmp(peername, pThis->permittedPeers.peer[i].name)) {
				*pbFoundPositiveMatch = 1;
				break;
			}
		} else {
			relpTcpChkOnePeerWildcard(pThis->permittedPeers.peer[i].wildcardRoot,
			        peername, pbFoundPositiveMatch);
			if (*pbFoundPositiveMatch)
				break;
		}
	}
}