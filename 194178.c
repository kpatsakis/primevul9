MsgSetFlowControlType(msg_t *pMsg, flowControl_t eFlowCtl)
{
	DEFiRet;
	assert(pMsg != NULL);
	assert(eFlowCtl == eFLOWCTL_NO_DELAY || eFlowCtl == eFLOWCTL_LIGHT_DELAY || eFlowCtl == eFLOWCTL_FULL_DELAY);

	pMsg->flowCtlType = eFlowCtl;

	RETiRet;
}