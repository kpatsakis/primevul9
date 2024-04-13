AddPermittedPeerWildcard(tcpPermittedPeerEntry_t *pEtry, char* pszStr, int lenStr)
{
	tcpPermittedPeerWildcardComp_t *pNew = NULL;
	int iSrc;
	int iDst;
	ENTER_RELPFUNC;

	if((pNew = calloc(1, sizeof(tcpPermittedPeerWildcardComp_t))) == NULL) {
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	if(lenStr == 0) {
		pNew->wildcardType = tcpPEER_WILDCARD_EMPTY_COMPONENT;
		FINALIZE;
	} else {
		/* alloc memory for the domain component. We may waste a byte or
		 * two, but that's ok.
		 */
		if((pNew->pszDomainPart = malloc(lenStr +1 )) == NULL) {
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
		}
	}

	if(pszStr[0] == '*') {
		pNew->wildcardType = tcpPEER_WILDCARD_AT_START;
		iSrc = 1; /* skip '*' */
	} else {
		iSrc = 0;
	}

	for(iDst = 0 ; iSrc < lenStr && pszStr[iSrc] != '*' ; ++iSrc, ++iDst)  {
		pNew->pszDomainPart[iDst] = pszStr[iSrc];
	}

	if(iSrc < lenStr) {
		if(iSrc + 1 == lenStr && pszStr[iSrc] == '*') {
			if(pNew->wildcardType == tcpPEER_WILDCARD_AT_START) {
				ABORT_FINALIZE(RELP_RET_INVLD_WILDCARD);
			} else {
				pNew->wildcardType = tcpPEER_WILDCARD_AT_END;
			}
		} else {
			/* we have an invalid wildcard, something follows the asterisk! */
			ABORT_FINALIZE(RELP_RET_INVLD_WILDCARD);
		}
	}

	if(lenStr == 1 && pNew->wildcardType == tcpPEER_WILDCARD_AT_START) {
		pNew->wildcardType = tcpPEER_WILDCARD_MATCH_ALL;
	}

	/* if we reach this point, we had a valid wildcard. We now need to
	 * properly terminate the domain component string.
	 */
	pNew->pszDomainPart[iDst] = '\0';
	pNew->lenDomainPart = strlen((char*)pNew->pszDomainPart);

finalize_it:
	if(iRet != RELP_RET_OK) {
		if(pNew != NULL) {
			if(pNew->pszDomainPart != NULL)
				free(pNew->pszDomainPart);
			free(pNew);
		}
	} else {
		/* add the element to linked list */
		if(pEtry->wildcardRoot == NULL) {
			pEtry->wildcardRoot = pNew;
			pEtry->wildcardLast = pNew;
		} else {
			pEtry->wildcardLast->pNext = pNew;
		}
		pEtry->wildcardLast = pNew;
	}
	LEAVE_RELPFUNC;
}