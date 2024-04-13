rsCStrConvertToBool(cstr_t *pStr, number_t *pBool)
{
	DEFiRet;

	ASSERT(pStr != NULL);
	ASSERT(pBool != NULL);

	iRet = rsCStrConvertToNumber(pStr, pBool);

	if(iRet != RS_RET_NOT_A_NUMBER) {
		FINALIZE; /* in any case, we have nothing left to do */
	}

	/* TODO: maybe we can do better than strcasecmp ;) -- overhead! */
	if(!strcasecmp((char*)rsCStrGetSzStr(pStr), "true")) {
		*pBool = 1;
	} else if(!strcasecmp((char*)rsCStrGetSzStr(pStr), "yes")) {
		*pBool = 1;
	} else {
		*pBool = 0;
	}

finalize_it:
	RETiRet;
}