int rsCStrCaseInsensitveStartsWithSzStr(cstr_t *pCS1, uchar *psz, size_t iLenSz)
{
	register size_t i;

	rsCHECKVALIDOBJECT(pCS1, OIDrsCStr);
	assert(psz != NULL);
	assert(iLenSz == strlen((char*)psz)); /* just make sure during debugging! */
	if(pCS1->iStrLen >= iLenSz) {
		/* we are using iLenSz below, because we need to check
		 * iLenSz characters at maximum (start with!)
		 */
		if(iLenSz == 0)
			return 0; /* yes, it starts with a zero-sized string ;) */
		else {  /* we now have something to compare, so let's do it... */
			for(i = 0 ; i < iLenSz ; ++i) {
				if(tolower(pCS1->pBuf[i]) != tolower(psz[i]))
					return tolower(pCS1->pBuf[i]) - tolower(psz[i]);
			}
			/* if we arrive here, the string actually starts with psz */
			return 0;
		}
	}
	else
		return -1; /* pCS1 is less then psz */
}