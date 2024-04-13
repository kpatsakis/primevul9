static void MsgDeleteMutexLockingCase(msg_t *pThis)
{
	assert(pThis != NULL);
	pthread_mutex_destroy(&pThis->mut);
}