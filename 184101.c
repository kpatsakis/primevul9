static int parseRFCField(uchar **pp2parse, uchar *pResult, int *pLenStr)
{
	uchar *p2parse;
	int iRet = 0;

	assert(pp2parse != NULL);
	assert(*pp2parse != NULL);
	assert(pResult != NULL);

	p2parse = *pp2parse;

	/* this is the actual parsing loop */
	while(*pLenStr > 0  && *p2parse != ' ') {
		*pResult++ = *p2parse++;
		--(*pLenStr);
	}

	if(*pLenStr > 0 && *p2parse == ' ') {
		++p2parse; /* eat SP, but only if not at end of string */
		--(*pLenStr);
	} else {
		iRet = 1; /* there MUST be an SP! */
	}
	*pResult = '\0';

	/* set the new parse pointer */
	*pp2parse = p2parse;
	return 0;
}