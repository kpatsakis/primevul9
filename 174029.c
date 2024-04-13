relpTcpPermittedPeerWildcardCompile(tcpPermittedPeerEntry_t *pEtry)
{
	char *pC;
	char *pStart;
	ENTER_RELPFUNC;

	/* first check if we have a wildcard */
	for(pC = pEtry->name ; *pC != '\0' && *pC != '*' ; ++pC)
		/*EMPTY, just skip*/;

	if(*pC == '\0') { /* no wildcard found, we are done */
		FINALIZE;
	}

	/* if we reach this point, the string contains wildcards. So let's
	 * compile the structure. To do so, we must parse from dot to dot
	 * and create a wildcard entry for each domain component we find.
	 * We must also flag problems if we have an asterisk in the middle
	 * of the text (it is supported at the start or end only).
	 */
	pC = pEtry->name;
	while(*pC) {
		pStart = pC;
		/* find end of domain component */
		for( ; *pC != '\0' && *pC != '.' ; ++pC)
			/*EMPTY, just skip*/;
		CHKRet(AddPermittedPeerWildcard(pEtry, pStart, pC - pStart));
		/* now check if we have an empty component at end of string */
		if(*pC == '.' && *(pC + 1) == '\0') {
			/* pStart is a dummy, it is not used if length is 0 */
			CHKRet(AddPermittedPeerWildcard(pEtry, pStart, 0));
		}
		if(*pC != '\0')
			++pC;
	}

finalize_it:
	LEAVE_RELPFUNC;
}