void MsgSetTAG(msg_t *pMsg, uchar* pszBuf, size_t lenBuf)
{
	uchar *pBuf;
	assert(pMsg != NULL);

	freeTAG(pMsg);

	pMsg->iLenTAG = lenBuf;
	if(pMsg->iLenTAG < CONF_TAG_BUFSIZE) {
		/* small enough: use fixed buffer (faster!) */
		pBuf = pMsg->TAG.szBuf;
	} else {
		if((pBuf = (uchar*) MALLOC(pMsg->iLenTAG + 1)) == NULL) {
			/* truncate message, better than completely loosing it... */
			pBuf = pMsg->TAG.szBuf;
			pMsg->iLenTAG = CONF_TAG_BUFSIZE - 1;
		} else {
			pMsg->TAG.pszTAG = pBuf;
		}
	}

	memcpy(pBuf, pszBuf, pMsg->iLenTAG);
	pBuf[pMsg->iLenTAG] = '\0'; /* this also works with truncation! */
}