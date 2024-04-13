xmlAutomataNewOnceTrans(xmlAutomataPtr am, xmlAutomataStatePtr from,
			 xmlAutomataStatePtr to, const xmlChar *token,
			 int min, int max, void *data) {
    xmlRegAtomPtr atom;
    int counter;

    if ((am == NULL) || (from == NULL) || (token == NULL))
	return(NULL);
    if (min < 1)
	return(NULL);
    if ((max < min) || (max < 1))
	return(NULL);
    atom = xmlRegNewAtom(am, XML_REGEXP_STRING);
    if (atom == NULL)
	return(NULL);
    atom->valuep = xmlStrdup(token);
    atom->data = data;
    atom->quant = XML_REGEXP_QUANT_ONCEONLY;
    atom->min = min;
    atom->max = max;
    /*
     * associate a counter to the transition.
     */
    counter = xmlRegGetCounter(am);
    am->counters[counter].min = 1;
    am->counters[counter].max = 1;

    /* xmlFAGenerateTransitions(am, from, to, atom); */
    if (to == NULL) {
	to = xmlRegNewState(am);
	xmlRegStatePush(am, to);
    }
    xmlRegStateAddTrans(am, from, atom, to, counter, -1);
    xmlRegAtomPush(am, atom);
    am->state = to;
    return(to);
}