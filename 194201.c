static inline void tryEmulateTAG(msg_t *pM, sbool bLockMutex)
{
	size_t lenTAG;
	uchar bufTAG[CONF_TAG_MAXSIZE];
	assert(pM != NULL);

	if(bLockMutex == LOCK_MUTEX)
		MsgLock(pM);
	if(pM->iLenTAG > 0)
		return; /* done, no need to emulate */
	
	if(getProtocolVersion(pM) == 1) {
		if(!strcmp(getPROCID(pM, MUTEX_ALREADY_LOCKED), "-")) {
			/* no process ID, use APP-NAME only */
			MsgSetTAG(pM, (uchar*) getAPPNAME(pM, MUTEX_ALREADY_LOCKED), getAPPNAMELen(pM, MUTEX_ALREADY_LOCKED));
		} else {
			/* now we can try to emulate */
			lenTAG = snprintf((char*)bufTAG, CONF_TAG_MAXSIZE, "%s[%s]",
					  getAPPNAME(pM, MUTEX_ALREADY_LOCKED), getPROCID(pM, MUTEX_ALREADY_LOCKED));
			bufTAG[32] = '\0'; /* just to make sure... */
			MsgSetTAG(pM, bufTAG, lenTAG);
		}
	}
	if(bLockMutex == LOCK_MUTEX)
		MsgUnlock(pM);
}