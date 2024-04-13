xmlAutomataGetInitState(xmlAutomataPtr am) {
    if (am == NULL)
	return(NULL);
    return(am->start);
}