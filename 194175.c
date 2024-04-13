static rsRetVal msgConstructFinalizer(msg_t *pThis)
{
	MsgPrepareEnqueue(pThis);
	return RS_RET_OK;
}