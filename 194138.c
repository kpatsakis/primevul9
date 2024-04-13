void MsgSetRcvFrom(msg_t *pThis, prop_t *new)
{
	assert(pThis != NULL);

	prop.AddRef(new);
	if(pThis->msgFlags & NEEDS_DNSRESOL) {
		if(pThis->rcvFrom.pfrominet != NULL)
		free(pThis->rcvFrom.pfrominet);
		pThis->msgFlags &= ~NEEDS_DNSRESOL;
	} else {
		if(pThis->rcvFrom.pRcvFrom != NULL)
			prop.Destruct(&pThis->rcvFrom.pRcvFrom);
	}
	pThis->rcvFrom.pRcvFrom = new;
}