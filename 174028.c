relpTcpFreePermittedPeers(relpTcp_t *pThis)
{
	int i;
	for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i)
		free(pThis->permittedPeers.peer[i].name);
	pThis->permittedPeers.nmemb = 0;
}