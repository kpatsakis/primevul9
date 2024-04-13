xmlRegFreeRegexp(xmlRegexpPtr regexp) {
    int i;
    if (regexp == NULL)
	return;

    if (regexp->string != NULL)
	xmlFree(regexp->string);
    if (regexp->states != NULL) {
	for (i = 0;i < regexp->nbStates;i++)
	    xmlRegFreeState(regexp->states[i]);
	xmlFree(regexp->states);
    }
    if (regexp->atoms != NULL) {
	for (i = 0;i < regexp->nbAtoms;i++)
	    xmlRegFreeAtom(regexp->atoms[i]);
	xmlFree(regexp->atoms);
    }
    if (regexp->counters != NULL)
	xmlFree(regexp->counters);
    if (regexp->compact != NULL)
	xmlFree(regexp->compact);
    if (regexp->transdata != NULL)
	xmlFree(regexp->transdata);
    if (regexp->stringMap != NULL) {
	for (i = 0; i < regexp->nbstrings;i++)
	    xmlFree(regexp->stringMap[i]);
	xmlFree(regexp->stringMap);
    }

    xmlFree(regexp);
}