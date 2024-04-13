rsRetVal cstrConstruct(cstr_t **ppThis)
{
	DEFiRet;
	cstr_t *pThis;

	ASSERT(ppThis != NULL);

	CHKmalloc(pThis = (cstr_t*) calloc(1, sizeof(cstr_t)));

	rsSETOBJTYPE(pThis, OIDrsCStr);
	pThis->pBuf = NULL;
	pThis->pszBuf = NULL;
	pThis->iBufSize = 0;
	pThis->iStrLen = 0;
	*ppThis = pThis;

finalize_it:
	RETiRet;
}