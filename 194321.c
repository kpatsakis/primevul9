rsRetVal rsCStrAppendInt(cstr_t *pThis, long i)
{
	DEFiRet;
	uchar szBuf[32];

	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);

	CHKiRet(srUtilItoA((char*) szBuf, sizeof(szBuf), i));

	iRet = rsCStrAppendStr(pThis, szBuf);
finalize_it:
	RETiRet;
}