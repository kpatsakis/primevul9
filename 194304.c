rsCStrConvertToNumber(cstr_t *pStr, number_t *pNumber)
{
	DEFiRet;
	number_t n;
	int bIsNegative;
	size_t i;

	ASSERT(pStr != NULL);
	ASSERT(pNumber != NULL);

	if(pStr->iStrLen == 0) {
		/* can be converted to 0! (by convention) */
		pNumber = 0;
		FINALIZE;
	}

	/* first skip whitespace (if present) */
	for(i = 0 ; i < pStr->iStrLen && isspace(pStr->pBuf[i]) ; ++i) {
		/*DO NOTHING*/
	}

	/* we have a string, so let's check its syntax */
	if(pStr->pBuf[i] == '+') {
		++i; /* skip that char */
		bIsNegative = 0;
	} else if(pStr->pBuf[0] == '-') {
		++i; /* skip that char */
		bIsNegative = 1;
	} else {
		bIsNegative = 0;
	}

	/* TODO: octal? hex? */
	n = 0;
	while(i < pStr->iStrLen && isdigit(pStr->pBuf[i])) {
		n = n * 10 + pStr->pBuf[i] - '0';
		++i;
	}
	
	if(i < pStr->iStrLen) /* non-digits before end of string? */
		ABORT_FINALIZE(RS_RET_NOT_A_NUMBER);

	if(bIsNegative)
		n *= -1;

	/* we got it, so return the number */
	*pNumber = n;

finalize_it:
	RETiRet;
}