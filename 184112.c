static int parseRFCStructuredData(uchar **pp2parse, uchar *pResult, int *pLenStr)
{
	uchar *p2parse;
	int bCont = 1;
	int iRet = 0;
	int lenStr;

	assert(pp2parse != NULL);
	assert(*pp2parse != NULL);
	assert(pResult != NULL);

	p2parse = *pp2parse;
	lenStr = *pLenStr;

	/* this is the actual parsing loop
	 * Remeber: structured data starts with [ and includes any characters
	 * until the first ] followed by a SP. There may be spaces inside
	 * structured data. There may also be \] inside the structured data, which
	 * do NOT terminate an element.
	 */
	if(lenStr == 0 || *p2parse != '[')
		return 1; /* this is NOT structured data! */

	if(*p2parse == '-') { /* empty structured data? */
		*pResult++ = '-';
		++p2parse;
		--lenStr;
	} else {
		while(bCont) {
			if(lenStr < 2) {
				/* we now need to check if we have only structured data */
				if(lenStr > 0 && *p2parse == ']') {
					*pResult++ = *p2parse;
					p2parse++;
					lenStr--;
					bCont = 0;
				} else {
					iRet = 1; /* this is not valid! */
					bCont = 0;
				}
			} else if(*p2parse == '\\' && *(p2parse+1) == ']') {
				/* this is escaped, need to copy both */
				*pResult++ = *p2parse++;
				*pResult++ = *p2parse++;
				lenStr -= 2;
			} else if(*p2parse == ']' && *(p2parse+1) == ' ') {
				/* found end, just need to copy the ] and eat the SP */
				*pResult++ = *p2parse;
				p2parse += 2;
				lenStr -= 2;
				bCont = 0;
			} else {
				*pResult++ = *p2parse++;
				--lenStr;
			}
		}
	}

	if(lenStr > 0 && *p2parse == ' ') {
		++p2parse; /* eat SP, but only if not at end of string */
		--lenStr;
	} else {
		iRet = 1; /* there MUST be an SP! */
	}
	*pResult = '\0';

	/* set the new parse pointer */
	*pp2parse = p2parse;
	*pLenStr = lenStr;
	return 0;
}