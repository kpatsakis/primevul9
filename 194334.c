rsRetVal rsCStrAppendStrWithLen(cstr_t *pThis, uchar* psz, size_t iStrLen)
{
	DEFiRet;

	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);
	assert(psz != NULL);

	/* does the string fit? */
	if(pThis->iStrLen + iStrLen > pThis->iBufSize) {  
		CHKiRet(rsCStrExtendBuf(pThis, iStrLen)); /* need more memory! */
	}

	/* ok, now we always have sufficient continues memory to do a memcpy() */
	memcpy(pThis->pBuf + pThis->iStrLen, psz, iStrLen);
	pThis->iStrLen += iStrLen;

finalize_it:
	RETiRet;
}