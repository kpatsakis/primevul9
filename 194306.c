rsRetVal cstrAppendCStr(cstr_t *pThis, cstr_t *pstrAppend)
{
	return rsCStrAppendStrWithLen(pThis, pstrAppend->pBuf, pstrAppend->iStrLen);
}