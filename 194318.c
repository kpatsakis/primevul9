int rsCStrCStrCmp(cstr_t *pCS1, cstr_t *pCS2)
{
	rsCHECKVALIDOBJECT(pCS1, OIDrsCStr);
	rsCHECKVALIDOBJECT(pCS2, OIDrsCStr);
	if(pCS1->iStrLen == pCS2->iStrLen)
		if(pCS1->iStrLen == 0)
			return 0; /* zero-sized string are equal ;) */
		else {  /* we now have two non-empty strings of equal
			 * length, so we need to actually check if they
			 * are equal.
			 */
			register size_t i;
			for(i = 0 ; i < pCS1->iStrLen ; ++i) {
				if(pCS1->pBuf[i] != pCS2->pBuf[i])
					return pCS1->pBuf[i] - pCS2->pBuf[i];
			}
			/* if we arrive here, the strings are equal */
			return 0;
		}
	else
		return pCS1->iStrLen - pCS2->iStrLen;
}