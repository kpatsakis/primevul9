relpTcpSetConnTimeout(relpTcp_t *pThis, int connTimeout)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->connTimeout = connTimeout;
	LEAVE_RELPFUNC;
}