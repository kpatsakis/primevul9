rsRetVal MsgSetRcvFromIPStr(msg_t *pThis, uchar *psz, int len, prop_t **ppProp)
{
	DEFiRet;
	assert(pThis != NULL);

	CHKiRet(prop.CreateOrReuseStringProp(ppProp, psz, len));
	MsgSetRcvFromIP(pThis, *ppProp);

finalize_it:
	RETiRet;
}