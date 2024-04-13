uchar *getRcvFrom(msg_t *pM)
{
	uchar *psz;
	int len;
	BEGINfunc

	if(pM == NULL) {
		psz = UCHAR_CONSTANT("");
	} else {
		resolveDNS(pM);
		if(pM->rcvFrom.pRcvFrom == NULL)
			psz = UCHAR_CONSTANT("");
		else
			prop.GetString(pM->rcvFrom.pRcvFrom, &psz, &len);
	}
	ENDfunc
	return psz;
}