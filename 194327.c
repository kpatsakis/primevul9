int cstrLen(cstr_t *pThis)
{
	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);
	return(pThis->iStrLen);
}