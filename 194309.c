int rsCStrSzStrCmp(cstr_t *pCS1, uchar *psz, size_t iLenSz)
{
	rsCHECKVALIDOBJECT(pCS1, OIDrsCStr);
	assert(psz != NULL);
	assert(iLenSz == strlen((char*)psz)); /* just make sure during debugging! */
	if(pCS1->iStrLen == iLenSz)
		/* we are using iLenSz below, because the lengths
		 * are equal and iLenSz is faster to access
		 */
		if(iLenSz == 0)
			return 0; /* zero-sized strings are equal ;) */
		else {  /* we now have two non-empty strings of equal
			 * length, so we need to actually check if they
			 * are equal.
			 */
			register size_t i;
			for(i = 0 ; i < iLenSz ; ++i) {
				if(pCS1->pBuf[i] != psz[i])
					return pCS1->pBuf[i] - psz[i];
			}
			/* if we arrive here, the strings are equal */
			return 0;
		}
	else
		return pCS1->iStrLen - iLenSz;
}