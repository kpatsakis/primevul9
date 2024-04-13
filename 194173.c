static int getAPPNAMELen(msg_t *pM, sbool bLockMutex)
{
	assert(pM != NULL);
	prepareAPPNAME(pM, bLockMutex);
	return (pM->pCSAPPNAME == NULL) ? 0 : rsCStrLen(pM->pCSAPPNAME);
}