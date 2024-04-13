getTAG(msg_t *pM, uchar **ppBuf, int *piLen)
{
	if(pM == NULL) {
		*ppBuf = UCHAR_CONSTANT("");
		*piLen = 0;
	} else {
		if(pM->iLenTAG == 0)
			tryEmulateTAG(pM, LOCK_MUTEX);
		if(pM->iLenTAG == 0) {
			*ppBuf = UCHAR_CONSTANT("");
			*piLen = 0;
		} else {
			*ppBuf = (pM->iLenTAG < CONF_TAG_BUFSIZE) ? pM->TAG.szBuf : pM->TAG.pszTAG;
			*piLen = pM->iLenTAG;
		}
	}
}