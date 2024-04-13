relpTcpSetDHBits(relpTcp_t *pThis, int bits)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->dhBits = bits;
	LEAVE_RELPFUNC;
}