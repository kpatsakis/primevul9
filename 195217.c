static rsRetVal resetConfigVariables(uchar __attribute__((unused)) *pp, void __attribute__((unused)) *pVal)
{
	if(pszBindAddr != NULL) {
		free(pszBindAddr);
		pszBindAddr = NULL;
	}
	if(udpLstnSocks != NULL) {
		net.closeUDPListenSockets(udpLstnSocks);
		udpLstnSocks = NULL;
	}
	return RS_RET_OK;
}