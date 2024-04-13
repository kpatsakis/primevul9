char *getProtocolVersionString(msg_t *pM)
{
	assert(pM != NULL);
	return(pM->iProtocolVersion ? "1" : "0");
}