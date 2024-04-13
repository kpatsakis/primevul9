xmlRegexpIsDeterminist(xmlRegexpPtr comp) {
    xmlAutomataPtr am;
    int ret;

    if (comp == NULL)
	return(-1);
    if (comp->determinist != -1)
	return(comp->determinist);

    am = xmlNewAutomata();
    if (am->states != NULL) {
	int i;

	for (i = 0;i < am->nbStates;i++)
	    xmlRegFreeState(am->states[i]);
	xmlFree(am->states);
    }
    am->nbAtoms = comp->nbAtoms;
    am->atoms = comp->atoms;
    am->nbStates = comp->nbStates;
    am->states = comp->states;
    am->determinist = -1;
    am->flags = comp->flags;
    ret = xmlFAComputesDeterminism(am);
    am->atoms = NULL;
    am->states = NULL;
    xmlFreeAutomata(am);
    comp->determinist = ret;
    return(ret);
}