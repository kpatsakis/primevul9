char *getPROCID(msg_t *pM, sbool bLockMutex)
{
	ISOBJ_TYPE_assert(pM, msg);
	preparePROCID(pM, bLockMutex);
	return (pM->pCSPROCID == NULL) ? "-" : (char*) cstrGetSzStrNoNULL(pM->pCSPROCID);
}