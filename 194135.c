void MsgSetHOSTNAME(msg_t *pThis, uchar* pszHOSTNAME, int lenHOSTNAME)
{
	assert(pThis != NULL);

	freeHOSTNAME(pThis);

	pThis->iLenHOSTNAME = lenHOSTNAME;
	if(pThis->iLenHOSTNAME < CONF_HOSTNAME_BUFSIZE) {
		/* small enough: use fixed buffer (faster!) */
		pThis->pszHOSTNAME = pThis->szHOSTNAME;
	} else if((pThis->pszHOSTNAME = (uchar*) MALLOC(pThis->iLenHOSTNAME + 1)) == NULL) {
		/* truncate message, better than completely loosing it... */
		pThis->pszHOSTNAME = pThis->szHOSTNAME;
		pThis->iLenHOSTNAME = CONF_HOSTNAME_BUFSIZE - 1;
	}

	memcpy(pThis->pszHOSTNAME, pszHOSTNAME, pThis->iLenHOSTNAME);
	pThis->pszHOSTNAME[pThis->iLenHOSTNAME] = '\0'; /* this also works with truncation! */
}