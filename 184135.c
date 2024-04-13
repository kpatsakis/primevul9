setCurrRuleset(void __attribute__((unused)) *pVal, uchar *pszName)
{
	ruleset_t *pRuleset;
	rsRetVal localRet;
	DEFiRet;

	localRet = ruleset.SetCurrRuleset(pszName);

	if(localRet == RS_RET_NOT_FOUND) {
		DBGPRINTF("begin new current rule set '%s'\n", pszName);
		CHKiRet(ruleset.Construct(&pRuleset));
		CHKiRet(ruleset.SetName(pRuleset, pszName));
		CHKiRet(ruleset.ConstructFinalize(pRuleset));
	} else {
		ABORT_FINALIZE(localRet);
	}

finalize_it:
	free(pszName); /* no longer needed */
	RETiRet;
}