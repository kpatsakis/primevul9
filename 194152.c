uchar *getProgramName(msg_t *pM, sbool bLockMutex)
{
	prepareProgramName(pM, bLockMutex);
	return (pM->pCSProgName == NULL) ? UCHAR_CONSTANT("") : rsCStrGetSzStrNoNULL(pM->pCSProgName);
}