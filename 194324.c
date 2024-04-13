int rsCStrLocateInSzStr(cstr_t *pThis, uchar *sz)
{
	int i;
	int iMax;
	int bFound;
	rsCHECKVALIDOBJECT(pThis, OIDrsCStr);
	assert(sz != NULL);
	
	if(pThis->iStrLen == 0)
		return 0;
	
	/* compute the largest index where a match could occur - after all,
	 * the to-be-located string must be able to be present in the 
	 * searched string (it needs its size ;)).
	 */
	iMax = strlen((char*)sz) - pThis->iStrLen;

	bFound = 0;
	i = 0;
	while(i  <= iMax && !bFound) {
		size_t iCheck;
		uchar *pComp = sz + i;
		for(iCheck = 0 ; iCheck < pThis->iStrLen ; ++iCheck)
			if(*(pComp + iCheck) != *(pThis->pBuf + iCheck))
				break;
		if(iCheck == pThis->iStrLen)
			bFound = 1; /* found! - else it wouldn't be equal */
		else
			++i; /* on to the next try */
	}

	return(bFound ? i : -1);
}