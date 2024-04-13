MsgSetAfterPRIOffs(msg_t *pMsg, short offs)
{
	assert(pMsg != NULL);
	pMsg->offAfterPRI = offs;
	return RS_RET_OK;
}