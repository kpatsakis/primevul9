char *getAPPNAME(msg_t *pM, sbool bLockMutex)
{
	assert(pM != NULL);
	prepareAPPNAME(pM, bLockMutex);
	return (pM->pCSAPPNAME == NULL) ? "" : (char*) rsCStrGetSzStrNoNULL(pM->pCSAPPNAME);
}