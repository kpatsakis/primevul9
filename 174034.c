relpTcpHintBurstBegin(relpTcp_t *pThis)
{
	setCORKopt(pThis->sock, 1);
}