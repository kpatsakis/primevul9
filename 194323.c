rsRetVal rsCStrConstructFromszStr(cstr_t **ppThis, uchar *sz)
{
	DEFiRet;
	cstr_t *pThis;

	assert(ppThis != NULL);

	CHKiRet(rsCStrConstruct(&pThis));

	pThis->iBufSize = pThis->iStrLen = strlen((char *) sz);
	if((pThis->pBuf = (uchar*) malloc(sizeof(uchar) * pThis->iStrLen)) == NULL) {
		RSFREEOBJ(pThis);
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);
	}

	/* we do NOT need to copy the \0! */
	memcpy(pThis->pBuf, sz, pThis->iStrLen);

	*ppThis = pThis;

finalize_it:
	RETiRet;
}