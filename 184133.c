static rsRetVal setActionResumeInterval(void __attribute__((unused)) *pVal, int iNewVal)
{
	return actionSetGlobalResumeInterval(iNewVal);
}