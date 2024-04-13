setDefaultRuleset(void __attribute__((unused)) *pVal, uchar *pszName)
{
	DEFiRet;

	CHKiRet(ruleset.SetDefaultRuleset(pszName));

finalize_it:
	free(pszName); /* no longer needed */
	RETiRet;
}