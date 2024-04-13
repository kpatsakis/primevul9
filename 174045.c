relpTcpSetUsrPtr(relpTcp_t *pThis, void *pUsr)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->pUsr = pUsr;
	LEAVE_RELPFUNC;
}