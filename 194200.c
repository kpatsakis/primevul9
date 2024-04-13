static inline int getPROCIDLen(msg_t *pM, sbool bLockMutex)
{
	assert(pM != NULL);
	preparePROCID(pM, bLockMutex);
	return (pM->pCSPROCID == NULL) ? 1 : rsCStrLen(pM->pCSPROCID);
}