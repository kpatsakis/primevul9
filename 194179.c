int getProgramNameLen(msg_t *pM, sbool bLockMutex)
{
	assert(pM != NULL);
	prepareProgramName(pM, bLockMutex);
	return (pM->pCSProgName == NULL) ? 0 : rsCStrLen(pM->pCSProgName);
}