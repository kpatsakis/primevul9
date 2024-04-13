destructAllActions(void)
{
	ruleset.DestructAllActions();
	bHaveMainQueue = 0; // flag that internal messages need to be temporarily stored
}