static rsRetVal aquireProgramName(msg_t *pM)
{
	register int i;
	uchar *pszTag;
	DEFiRet;

	assert(pM != NULL);
	if(pM->pCSProgName == NULL) {
		/* ok, we do not yet have it. So let's parse the TAG to obtain it.  */
		pszTag = (uchar*) ((pM->iLenTAG < CONF_TAG_BUFSIZE) ? pM->TAG.szBuf : pM->TAG.pszTAG);
		CHKiRet(cstrConstruct(&pM->pCSProgName));
		for(  i = 0
		    ; (i < pM->iLenTAG) && isprint((int) pszTag[i])
		      && (pszTag[i] != '\0') && (pszTag[i] != ':')
		      && (pszTag[i] != '[')  && (pszTag[i] != '/')
		    ; ++i) {
			CHKiRet(cstrAppendChar(pM->pCSProgName, pszTag[i]));
		}
		CHKiRet(cstrFinalize(pM->pCSProgName));
	}
finalize_it:
	RETiRet;
}