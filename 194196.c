getRcvFromIP(msg_t *pM)
{
	uchar *psz;
	int len;
	BEGINfunc
	if(pM == NULL) {
		psz = UCHAR_CONSTANT("");
	} else {
		resolveDNS(pM); /* make sure we have a resolved entry */
		if(pM->pRcvFromIP == NULL)
			psz = UCHAR_CONSTANT("");
		else
			prop.GetString(pM->pRcvFromIP, &psz, &len);
	}
	ENDfunc
	return psz;
}