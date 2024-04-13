static void MsgPrepareEnqueueLockingCase(msg_t *pThis)
{
	BEGINfunc
	assert(pThis != NULL);
	pthread_mutex_init(&pThis->mut, NULL);
	pThis->bDoLock = 1;
	ENDfunc
}