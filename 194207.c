int getMSGLen(msg_t *pM)
{
	return((pM == NULL) ? 0 : pM->iLenMSG);
}