static void MsgLockLockingCase(msg_t *pThis)
{
	/* DEV debug only! dbgprintf("MsgLock(0x%lx)\n", (unsigned long) pThis); */
	assert(pThis != NULL);
	if(pThis->bDoLock == 1) /* TODO: this is a testing hack, we should find a way with better performance! -- rgerhards, 2009-01-27 */
		pthread_mutex_lock(&pThis->mut);
}