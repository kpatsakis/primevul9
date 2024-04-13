uchar *getMSG(msg_t *pM)
{
	uchar *ret;
	if(pM == NULL)
		ret = UCHAR_CONSTANT("");
	else {
		if(pM->iLenMSG == 0)
			ret = UCHAR_CONSTANT("");
		else
			ret = pM->pszRawMsg + pM->offMSG;
	}
	return ret;
}