int rsCStrOffsetSzStrCmp(cstr_t *pCS1, size_t iOffset, uchar *psz, size_t iLenSz)
{
	BEGINfunc
	rsCHECKVALIDOBJECT(pCS1, OIDrsCStr);
	assert(iOffset < pCS1->iStrLen);
	assert(psz != NULL);
	assert(iLenSz == strlen((char*)psz)); /* just make sure during debugging! */
	if((pCS1->iStrLen - iOffset) == iLenSz) {
		/* we are using iLenSz below, because the lengths
		 * are equal and iLenSz is faster to access
		 */
		if(iLenSz == 0) {
			return 0; /* zero-sized strings are equal ;) */
			ENDfunc
		} else {  /* we now have two non-empty strings of equal
			 * length, so we need to actually check if they
			 * are equal.
			 */
			register size_t i;
			for(i = 0 ; i < iLenSz ; ++i) {
				if(pCS1->pBuf[i+iOffset] != psz[i])
					return pCS1->pBuf[i+iOffset] - psz[i];
			}
			/* if we arrive here, the strings are equal */
			return 0;
			ENDfunc
		}
	}
	else {
		return pCS1->iStrLen - iOffset - iLenSz;
		ENDfunc
	}
}