relpTcpEnableTLS(relpTcp_t __attribute__((unused)) *pThis)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
#ifdef ENABLE_TLS
	pThis->bEnableTLS = 1;
#else
	iRet = RELP_RET_ERR_NO_TLS;
#endif /* #ifdef ENABLE_TLS */
	LEAVE_RELPFUNC;
}