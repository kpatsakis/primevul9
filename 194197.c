	if(currRefCount == 0)
	{
		/* DEV Debugging Only! dbgprintf("msgDestruct\t0x%lx, RefCount now 0, doing DESTROY\n", (unsigned long)pThis); */
		if(pThis->pszRawMsg != pThis->szRawMsg)
			free(pThis->pszRawMsg);
		freeTAG(pThis);
		freeHOSTNAME(pThis);
		if(pThis->pInputName != NULL)
			prop.Destruct(&pThis->pInputName);
		if((pThis->msgFlags & NEEDS_DNSRESOL) == 0) {
			if(pThis->rcvFrom.pRcvFrom != NULL)
				prop.Destruct(&pThis->rcvFrom.pRcvFrom);
		} else {
			free(pThis->rcvFrom.pfrominet);
		}
		if(pThis->pRcvFromIP != NULL)
			prop.Destruct(&pThis->pRcvFromIP);
		free(pThis->pszRcvdAt3164);
		free(pThis->pszRcvdAt3339);
		free(pThis->pszRcvdAt_MySQL);
		free(pThis->pszRcvdAt_PgSQL);
		free(pThis->pszTIMESTAMP_MySQL);
		free(pThis->pszTIMESTAMP_PgSQL);
		if(pThis->pCSProgName != NULL)
			rsCStrDestruct(&pThis->pCSProgName);
		if(pThis->pCSStrucData != NULL)
			rsCStrDestruct(&pThis->pCSStrucData);
		if(pThis->pCSAPPNAME != NULL)
			rsCStrDestruct(&pThis->pCSAPPNAME);
		if(pThis->pCSPROCID != NULL)
			rsCStrDestruct(&pThis->pCSPROCID);
		if(pThis->pCSMSGID != NULL)
			rsCStrDestruct(&pThis->pCSMSGID);
#	ifndef HAVE_ATOMIC_BUILTINS
		MsgUnlock(pThis);
# 	endif
		funcDeleteMutex(pThis);
		/* now we need to do our own optimization. Testing has shown that at least the glibc
		 * malloc() subsystem returns memory to the OS far too late in our case. So we need
		 * to help it a bit, by calling malloc_trim(), which will tell the alloc subsystem
		 * to consolidate and return to the OS. We keep 128K for our use, as a safeguard
		 * to too-frequent reallocs. But more importantly, we call this hook only every
		 * 100,000 messages (which is an approximation, as we do not work with atomic
		 * operations on the counter. --- rgerhards, 2009-06-22.
		 */
#		if HAVE_MALLOC_TRIM
		{	/* standard C requires a new block for a new variable definition!
			 * To simplify matters, we use modulo arithmetic and live with the fact
			 * that we trim too often when the counter wraps.
			 */
			static unsigned iTrimCtr = 1;
			currCnt = ATOMIC_INC_AND_FETCH_unsigned(&iTrimCtr, &mutTrimCtr);
			if(currCnt % 100000 == 0) {
				malloc_trim(128*1024);
			}
		}
#		endif
	} else {