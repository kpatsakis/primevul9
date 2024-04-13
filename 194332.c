void rsCStrDestruct(cstr_t **ppThis)
{
	cstr_t *pThis = *ppThis;

	free(pThis->pBuf);
	free(pThis->pszBuf);
	RSFREEOBJ(pThis);
	*ppThis = NULL;
}