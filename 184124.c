doFlushRptdMsgs(void)
{
	ruleset.IterateAllActions(flushRptdMsgsActions, NULL);
}