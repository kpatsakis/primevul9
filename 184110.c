multiSubmitMsg(multi_submit_t *pMultiSub)
{
	int i;
	DEFiRet;
	assert(pMultiSub != NULL);

	for(i = 0 ; i < pMultiSub->nElem ; ++i) {
		MsgPrepareEnqueue(pMultiSub->ppMsgs[i]);
	}

	iRet = qqueueMultiEnqObj(pMsgQueue, pMultiSub);
	pMultiSub->nElem = 0;

	RETiRet;
}