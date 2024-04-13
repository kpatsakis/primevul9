static int getPRIi(msg_t *pM)
{
	return (pM->iFacility << 3) + (pM->iSeverity);
}