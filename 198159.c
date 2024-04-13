xmlAutomataNewAllTrans(xmlAutomataPtr am, xmlAutomataStatePtr from,
		       xmlAutomataStatePtr to, int lax) {
    if ((am == NULL) || (from == NULL))
	return(NULL);
    xmlFAGenerateAllTransition(am, from, to, lax);
    if (to == NULL)
	return(am->state);
    return(to);
}