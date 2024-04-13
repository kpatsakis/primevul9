static inline void freeHOSTNAME(msg_t *pThis)
{
	if(pThis->iLenHOSTNAME >= CONF_HOSTNAME_BUFSIZE)
		free(pThis->pszHOSTNAME);
}