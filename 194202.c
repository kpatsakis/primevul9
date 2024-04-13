rsRetVal MsgReplaceMSG(msg_t *pThis, uchar* pszMSG, int lenMSG)
{
	int lenNew;
	uchar *bufNew;
	DEFiRet;
	ISOBJ_TYPE_assert(pThis, msg);
	assert(pszMSG != NULL);

	lenNew = pThis->iLenRawMsg + lenMSG - pThis->iLenMSG;
	if(lenMSG > pThis->iLenMSG && lenNew >= CONF_RAWMSG_BUFSIZE) {
		/*  we have lost our "bet" and need to alloc a new buffer ;) */
		CHKmalloc(bufNew = MALLOC(lenNew + 1));
		memcpy(bufNew, pThis->pszRawMsg, pThis->offMSG);
		if(pThis->pszRawMsg != pThis->szRawMsg)
			free(pThis->pszRawMsg);
		pThis->pszRawMsg = bufNew;
	}

	if(lenMSG > 0)
		memcpy(pThis->pszRawMsg + pThis->offMSG, pszMSG, lenMSG);
	pThis->pszRawMsg[lenNew] = '\0'; /* this also works with truncation! */
	pThis->iLenRawMsg = lenNew;
	pThis->iLenMSG = lenMSG;

finalize_it:
	RETiRet;
}