rsRetVal cstrTrimTrailingWhiteSpace(cstr_t *pThis)
{
	register int i;
	register uchar *pC;
	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);

	i = pThis->iStrLen;
	pC = pThis->pBuf + i - 1;
	while(i > 0 && isspace((int)*pC)) {
		--pC;
		--i;
	}
	/* i now is the new string length! */
	pThis->iStrLen = i;
	pThis->pBuf[pThis->iStrLen] = '0'; /* we always have this space */

	return RS_RET_OK;
}