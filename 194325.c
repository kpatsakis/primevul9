uchar*  rsCStrGetSzStrNoNULL(cstr_t *pThis)
{
	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);
	if(pThis->pBuf == NULL)
		return (uchar*) "";
	else
		return rsCStrGetSzStr(pThis);
}