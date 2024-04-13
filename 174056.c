relpTcpChkOnePeerWildcard(tcpPermittedPeerWildcardComp_t *pRoot, char *peername, int *pbFoundPositiveMatch)
{
	tcpPermittedPeerWildcardComp_t *pWildcard;
	char *pC;
	char *pStart; /* start of current domain component */
	int iWildcard, iName; /* work indexes for backward comparisons */

	*pbFoundPositiveMatch = 0;
	pWildcard = pRoot;
	pC = peername;
	while(*pC != '\0') {
		if(pWildcard == NULL) {
			/* we have more domain components than we have wildcards --> no match */
			goto done;
		}
		pStart = pC;
		while(*pC != '\0' && *pC != '.') {
			++pC;
		}

		/* got the component, now do the match */
		switch(pWildcard->wildcardType) {
			case tcpPEER_WILDCARD_NONE:
				if(   pWildcard->lenDomainPart != pC - pStart
				   || strncmp((char*)pStart, (char*)pWildcard->pszDomainPart, pC - pStart)) {
					goto done;
				}
				break;
			case tcpPEER_WILDCARD_AT_START:
				/* we need to do the backwards-matching manually */
				if(pWildcard->lenDomainPart > pC - pStart) {
					goto done;
				}
				iName = (size_t) (pC - pStart) - pWildcard->lenDomainPart;
				iWildcard = 0;
				while(iWildcard < pWildcard->lenDomainPart) {
					if(pWildcard->pszDomainPart[iWildcard] != pStart[iName]) {
						goto done;
					}
					++iName;
					++iWildcard;
				}
				break;
			case tcpPEER_WILDCARD_AT_END:
				if(   pWildcard->lenDomainPart > pC - pStart
				   || strncmp((char*)pStart, (char*)pWildcard->pszDomainPart,
					pWildcard->lenDomainPart)) {
					goto done;
				}
				break;
			case tcpPEER_WILDCARD_MATCH_ALL:
				/* everything is OK, just continue */
				break;
			case tcpPEER_WILDCARD_EMPTY_COMPONENT:
				if(pC - pStart > 0) {
				   	/* if it is not empty, it is no match... */
					goto done;
				}
				break;
		}
		pWildcard =  pWildcard->pNext; /* we processed this entry */

		/* skip '.' if we had it and so prepare for next iteration */
		if(*pC == '.')
			++pC;
	}
	
	/* we need to adjust for a border case, that is if the last component is
	 * empty. That happens frequently if the domain root (e.g. "example.com.")
	 * is properly given.
	 */
	if(pWildcard != NULL && pWildcard->wildcardType == tcpPEER_WILDCARD_EMPTY_COMPONENT)
		pWildcard = pWildcard->pNext;

	if(pWildcard != NULL) {
		/* we have more domain components than in the name to be
		 * checked. So this is no match.
		 */
		goto done;
	}
	*pbFoundPositiveMatch = 1;
done:	return;
}