diagGetMainMsgQSize(int *piSize)
{
	DEFiRet;
	assert(piSize != NULL);
	*piSize = (pMsgQueue->pqDA != NULL) ? pMsgQueue->pqDA->iQueueSize : 0;
	*piSize += pMsgQueue->iQueueSize;
	RETiRet;
}