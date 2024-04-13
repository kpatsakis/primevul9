submitMsg(msg_t *pMsg)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pMsg, msg);
	
	MsgPrepareEnqueue(pMsg);
	qqueueEnqObj(pMsgQueue, pMsg->flowCtlType, (void*) pMsg);

	RETiRet;
}