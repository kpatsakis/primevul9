xmlAutomataNewState(xmlAutomataPtr am) {
    xmlAutomataStatePtr to;

    if (am == NULL)
	return(NULL);
    to = xmlRegNewState(am);
    xmlRegStatePush(am, to);
    return(to);
}