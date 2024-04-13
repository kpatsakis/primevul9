int rsCStrSzStrStartsWithCStr(cstr_t *pCS1, uchar *psz, size_t iLenSz)
{
	register int i;
	int iMax;

	rsCHECKVALIDOBJECT(pCS1, OIDrsCStr);
	assert(psz != NULL);
	assert(iLenSz == strlen((char*)psz)); /* just make sure during debugging! */
	if(iLenSz >= pCS1->iStrLen) {
		/* we need to checkusing pCS1->iStrLen charactes at maximum, thus
		 * we move it to iMax.
		 */
		iMax = pCS1->iStrLen;
		if(iMax == 0)
			return 0; /* yes, it starts with a zero-sized string ;) */
		else {  /* we now have something to compare, so let's do it... */
			for(i = 0 ; i < iMax ; ++i) {
				if(psz[i] != pCS1->pBuf[i])
					return psz[i] - pCS1->pBuf[i];
			}
			/* if we arrive here, the string actually starts with pCS1 */
			return 0;
		}
	}
	else
		return -1; /* pCS1 is less then psz */
}