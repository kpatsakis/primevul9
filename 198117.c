xmlAutomataNewCountTrans(xmlAutomataPtr am, xmlAutomataStatePtr from,
			 xmlAutomataStatePtr to, const xmlChar *token,
			 int min, int max, void *data) {
    xmlRegAtomPtr atom;
    int counter;

    if ((am == NULL) || (from == NULL) || (token == NULL))
	return(NULL);
    if (min < 0)
	return(NULL);
    if ((max < min) || (max < 1))
	return(NULL);
    atom = xmlRegNewAtom(am, XML_REGEXP_STRING);
    if (atom == NULL)
	return(NULL);
    atom->valuep = xmlStrdup(token);
    atom->data = data;
    if (min == 0)
	atom->min = 1;
    else
	atom->min = min;
    atom->max = max;

    /*
     * associate a counter to the transition.
     */
    counter = xmlRegGetCounter(am);
    am->counters[counter].min = min;
    am->counters[counter].max = max;

    /* xmlFAGenerateTransitions(am, from, to, atom); */
    if (to == NULL) {
        to = xmlRegNewState(am);
	xmlRegStatePush(am, to);
    }
    xmlRegStateAddTrans(am, from, atom, to, counter, -1);
    xmlRegAtomPush(am, atom);
    am->state = to;

    if (to == NULL)
	to = am->state;
    if (to == NULL)
	return(NULL);
    if (min == 0)
	xmlFAGenerateEpsilonTransition(am, from, to);
    return(to);
}