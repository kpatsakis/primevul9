isAnonAuth(relpTcp_t *pThis)
{
	return pThis->ownCertFile == NULL;
}