void MsgSetRawMsg(msg_t *pThis, char* pszRawMsg, size_t lenMsg)
{
	assert(pThis != NULL);
	if(pThis->pszRawMsg != pThis->szRawMsg)
		free(pThis->pszRawMsg);

	pThis->iLenRawMsg = lenMsg;
	if(pThis->iLenRawMsg < CONF_RAWMSG_BUFSIZE) {
		/* small enough: use fixed buffer (faster!) */
		pThis->pszRawMsg = pThis->szRawMsg;
	} else if((pThis->pszRawMsg = (uchar*) MALLOC(pThis->iLenRawMsg + 1)) == NULL) {
		/* truncate message, better than completely loosing it... */
		pThis->pszRawMsg = pThis->szRawMsg;
		pThis->iLenRawMsg = CONF_RAWMSG_BUFSIZE - 1;
	}

	memcpy(pThis->pszRawMsg, pszRawMsg, pThis->iLenRawMsg);
	pThis->pszRawMsg[pThis->iLenRawMsg] = '\0'; /* this also works with truncation! */
}