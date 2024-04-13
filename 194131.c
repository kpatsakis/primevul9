void MsgSetRcvFromStr(msg_t *pThis, uchar *psz, int len, prop_t **ppProp)
{
	assert(pThis != NULL);
	assert(ppProp != NULL);

	prop.CreateOrReuseStringProp(ppProp, psz, len);
	MsgSetRcvFrom(pThis, *ppProp);
}