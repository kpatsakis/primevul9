static inline rsRetVal msgBaseConstruct(msg_t **ppThis)
{
	DEFiRet;
	msg_t *pM;

	assert(ppThis != NULL);
	CHKmalloc(pM = MALLOC(sizeof(msg_t)));
	objConstructSetObjInfo(pM); /* intialize object helper entities */

	/* initialize members in ORDER they appear in structure (think "cache line"!) */
	pM->flowCtlType = 0;
	pM->bDoLock = 0;
	pM->iRefCount = 1;
	pM->iSeverity = -1;
	pM->iFacility = -1;
	pM->offAfterPRI = 0;
	pM->offMSG = -1;
	pM->iProtocolVersion = 0;
	pM->msgFlags = 0;
	pM->iLenRawMsg = 0;
	pM->iLenMSG = 0;
	pM->iLenTAG = 0;
	pM->iLenHOSTNAME = 0;
	pM->pszRawMsg = NULL;
	pM->pszHOSTNAME = NULL;
	pM->pszRcvdAt3164 = NULL;
	pM->pszRcvdAt3339 = NULL;
	pM->pszRcvdAt_MySQL = NULL;
        pM->pszRcvdAt_PgSQL = NULL;
	pM->pszTIMESTAMP3164 = NULL;
	pM->pszTIMESTAMP3339 = NULL;
	pM->pszTIMESTAMP_MySQL = NULL;
        pM->pszTIMESTAMP_PgSQL = NULL;
	pM->pCSProgName = NULL;
	pM->pCSStrucData = NULL;
	pM->pCSAPPNAME = NULL;
	pM->pCSPROCID = NULL;
	pM->pCSMSGID = NULL;
	pM->pInputName = NULL;
	pM->pRcvFromIP = NULL;
	pM->rcvFrom.pRcvFrom = NULL;
	pM->pRuleset = NULL;
	memset(&pM->tRcvdAt, 0, sizeof(pM->tRcvdAt));
	memset(&pM->tTIMESTAMP, 0, sizeof(pM->tTIMESTAMP));
	pM->TAG.pszTAG = NULL;
	pM->pszTimestamp3164[0] = '\0';
	pM->pszTimestamp3339[0] = '\0';
	pM->pszTIMESTAMP_SecFrac[0] = '\0';
	pM->pszRcvdAt_SecFrac[0] = '\0';

	/* DEV debugging only! dbgprintf("msgConstruct\t0x%x, ref 1\n", (int)pM);*/

	*ppThis = pM;

finalize_it:
	RETiRet;
}