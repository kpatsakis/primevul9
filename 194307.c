rsRetVal rsCStrSetSzStr(cstr_t *pThis, uchar *pszNew)
{
	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);

	free(pThis->pBuf);
	free(pThis->pszBuf);
	if(pszNew == NULL) {
		pThis->iStrLen = 0;
		pThis->iBufSize = 0;
		pThis->pBuf = NULL;
		pThis->pszBuf = NULL;
	} else {
		pThis->iStrLen = strlen((char*)pszNew);
		pThis->iBufSize = pThis->iStrLen;
		pThis->pszBuf = NULL;

		/* now save the new value */
		if((pThis->pBuf = (uchar*) malloc(sizeof(uchar) * pThis->iStrLen)) == NULL) {
			RSFREEOBJ(pThis);
			return RS_RET_OUT_OF_MEMORY;
		}

		/* we do NOT need to copy the \0! */
		memcpy(pThis->pBuf, pszNew, pThis->iStrLen);
	}

	return RS_RET_OK;
}