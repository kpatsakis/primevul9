rsRetVal MsgSetRcvFromIP(msg_t *pThis, prop_t *new)
{
	assert(pThis != NULL);

	BEGINfunc
	prop.AddRef(new);
	if(pThis->pRcvFromIP != NULL)
		prop.Destruct(&pThis->pRcvFromIP);
	pThis->pRcvFromIP = new;
	ENDfunc
	return RS_RET_OK;
}