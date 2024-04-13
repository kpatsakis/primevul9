rsRetVal rsCStrAppendStr(cstr_t *pThis, uchar* psz)
{
	return rsCStrAppendStrWithLen(pThis, psz, strlen((char*) psz));
}