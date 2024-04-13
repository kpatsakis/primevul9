static inline void prepareAPPNAME(msg_t *pM, sbool bLockMutex)
{
	if(pM->pCSAPPNAME == NULL) {
		if(bLockMutex == LOCK_MUTEX)
			MsgLock(pM);

		/* re-query as things might have changed during locking */
		if(pM->pCSAPPNAME == NULL)
			tryEmulateAPPNAME(pM);

		if(bLockMutex == LOCK_MUTEX)
			MsgUnlock(pM);
	}
}