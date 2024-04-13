static inline void freeTAG(msg_t *pThis)
{
	if(pThis->iLenTAG >= CONF_TAG_BUFSIZE)
		free(pThis->TAG.pszTAG);
}