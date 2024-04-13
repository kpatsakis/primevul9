static inline void preparePROCID(msg_t *pM, sbool bLockMutex)
{
	if(pM->pCSPROCID == NULL) {
		if(bLockMutex == LOCK_MUTEX)
			MsgLock(pM);
		/* re-query, things may have changed in the mean time... */
		if(pM->pCSPROCID == NULL)
			aquirePROCIDFromTAG(pM);
		if(bLockMutex == LOCK_MUTEX)
			MsgUnlock(pM);
	}
}