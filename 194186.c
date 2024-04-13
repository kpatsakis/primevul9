int getHOSTNAMELen(msg_t *pM)
{
	if(pM == NULL)
		return 0;
	else
		if(pM->pszHOSTNAME == NULL) {
			resolveDNS(pM);
			if(pM->rcvFrom.pRcvFrom == NULL)
				return 0;
			else
				return prop.GetStringLen(pM->rcvFrom.pRcvFrom);
		} else
			return pM->iLenHOSTNAME;
}