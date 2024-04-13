msgConsumer(void __attribute__((unused)) *notNeeded, void *pUsr)
{
	DEFiRet;
	msg_t *pMsg = (msg_t*) pUsr;

	assert(pMsg != NULL);

	if((pMsg->msgFlags & NEEDS_PARSING) != 0) {
		parseMsg(pMsg);
	}
	ruleset.ProcessMsg(pMsg);
	msgDestruct(&pMsg);

	RETiRet;
}