rsRetVal rsCStrConstructFromCStr(cstr_t **ppThis, cstr_t *pFrom)
{
	DEFiRet;
	cstr_t *pThis;

	assert(ppThis != NULL);
	rsCHECKVALIDOBJECT(pFrom, OIDrsCStr);

	CHKiRet(rsCStrConstruct(&pThis));

	pThis->iBufSize = pThis->iStrLen = pFrom->iStrLen;
	if((pThis->pBuf = (uchar*) malloc(sizeof(uchar) * pThis->iStrLen)) == NULL) {
		RSFREEOBJ(pThis);
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);
	}

	/* copy properties */
	memcpy(pThis->pBuf, pFrom->pBuf, pThis->iStrLen);

	*ppThis = pThis;
finalize_it:
	RETiRet;
}