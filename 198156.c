xmlAutomataNewCountTrans2(xmlAutomataPtr am, xmlAutomataStatePtr from,
			 xmlAutomataStatePtr to, const xmlChar *token,
			 const xmlChar *token2,
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
    if ((token2 == NULL) || (*token2 == 0)) {
	atom->valuep = xmlStrdup(token);
    } else {
	int lenn, lenp;
	xmlChar *str;

	lenn = strlen((char *) token2);
	lenp = strlen((char *) token);

	str = (xmlChar *) xmlMallocAtomic(lenn + lenp + 2);
	if (str == NULL) {
	    xmlRegFreeAtom(atom);
	    return(NULL);
	}
	memcpy(&str[0], token, lenp);
	str[lenp] = '|';
	memcpy(&str[lenp + 1], token2, lenn);
	str[lenn + lenp + 1] = 0;

	atom->valuep = str;
    }
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