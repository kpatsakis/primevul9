relpTcpHintBurstEnd(relpTcp_t *pThis)
{
	setCORKopt(pThis->sock, 0);
}