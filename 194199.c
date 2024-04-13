getInputName(msg_t *pM, uchar **ppsz, int *plen)
{
	BEGINfunc
	if(pM == NULL || pM->pInputName == NULL) {
		*ppsz = UCHAR_CONSTANT("");
		*plen = 0;
	} else {
		prop.GetString(pM->pInputName, ppsz, plen);
	}
	ENDfunc
}