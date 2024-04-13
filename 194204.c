void MsgSetMSGoffs(msg_t *pMsg, short offs)
{
	ISOBJ_TYPE_assert(pMsg, msg);
	pMsg->offMSG = offs;
	if(offs > pMsg->iLenRawMsg) {
		assert(offs - 1 == pMsg->iLenRawMsg);
		pMsg->iLenMSG = 0;
	} else {
		pMsg->iLenMSG = pMsg->iLenRawMsg - offs;
	}
}