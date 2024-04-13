char *getHOSTNAME(msg_t *pM)
{
	if(pM == NULL)
		return "";
	else
		if(pM->pszHOSTNAME == NULL) {
			resolveDNS(pM);
			if(pM->rcvFrom.pRcvFrom == NULL) {
				return "";
			} else {
				uchar *psz;
				int len;
				prop.GetString(pM->rcvFrom.pRcvFrom, &psz, &len);
				return (char*) psz;
			}
		} else {
			return (char*) pM->pszHOSTNAME;
		}
}