static rsRetVal aquirePROCIDFromTAG(msg_t *pM)
{
	register int i;
	uchar *pszTag;
	DEFiRet;

	assert(pM != NULL);

	if(pM->pCSPROCID != NULL)
		return RS_RET_OK; /* we are already done ;) */

	if(getProtocolVersion(pM) != 0)
		return RS_RET_OK; /* we can only emulate if we have legacy format */

	pszTag = (uchar*) ((pM->iLenTAG < CONF_TAG_BUFSIZE) ? pM->TAG.szBuf : pM->TAG.pszTAG);

	/* find first '['... */
	i = 0;
	while((i < pM->iLenTAG) && (pszTag[i] != '['))
		++i;
	if(!(i < pM->iLenTAG))
		return RS_RET_OK;	/* no [, so can not emulate... */
	
	++i; /* skip '[' */

	/* now obtain the PROCID string... */
	CHKiRet(cstrConstruct(&pM->pCSPROCID));
	while((i < pM->iLenTAG) && (pszTag[i] != ']')) {
		CHKiRet(cstrAppendChar(pM->pCSPROCID, pszTag[i]));
		++i;
	}

	if(!(i < pM->iLenTAG)) {
		/* oops... it looked like we had a PROCID, but now it has
		 * turned out this is not true. In this case, we need to free
		 * the buffer and simply return. Note that this is NOT an error
		 * case!
		 */
		cstrDestruct(&pM->pCSPROCID);
		FINALIZE;
	}

	/* OK, finaally we could obtain a PROCID. So let's use it ;) */
	CHKiRet(cstrFinalize(pM->pCSPROCID));

finalize_it:
	RETiRet;
}