rsRetVal rsCStrTruncate(cstr_t *pThis, size_t nTrunc)
{
	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);

	if(pThis->iStrLen < nTrunc)
		return RS_TRUNCAT_TOO_LARGE;
	
	pThis->iStrLen -= nTrunc;

	if(pThis->pszBuf != NULL) {
		/* in this case, we adjust the psz representation
		 * by writing a new \0 terminator - this is by far
		 * the fastest way and outweights the additional memory
		 * required. 2005-9-19 rgerhards.
		 */
		 pThis->pszBuf[pThis->iStrLen] = '\0';
	}

	return RS_RET_OK;
}