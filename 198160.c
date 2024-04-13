xmlAutomataNewEpsilon(xmlAutomataPtr am, xmlAutomataStatePtr from,
		      xmlAutomataStatePtr to) {
    if ((am == NULL) || (from == NULL))
	return(NULL);
    xmlFAGenerateEpsilonTransition(am, from, to);
    if (to == NULL)
	return(am->state);
    return(to);
}