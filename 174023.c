relpTcpSetAuthMode(relpTcp_t *pThis, relpAuthMode_t authmode)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->authmode = authmode;
	LEAVE_RELPFUNC;
}