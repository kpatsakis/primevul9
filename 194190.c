getRawMsg(msg_t *pM, uchar **pBuf, int *piLen)
{
	if(pM == NULL) {
		*pBuf=  UCHAR_CONSTANT("");
		*piLen = 0;
	} else {
		if(pM->pszRawMsg == NULL) {
			*pBuf=  UCHAR_CONSTANT("");
			*piLen = 0;
		} else {
			*pBuf = pM->pszRawMsg;
			*piLen = pM->iLenRawMsg;
		}
	}
}