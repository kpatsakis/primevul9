void MsgSetRuleset(msg_t *pMsg, ruleset_t *pRuleset)
{
	assert(pMsg != NULL);
	pMsg->pRuleset = pRuleset;
}