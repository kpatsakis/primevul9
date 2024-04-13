xmlAutomataNewTransition(xmlAutomataPtr am, xmlAutomataStatePtr from,
			 xmlAutomataStatePtr to, const xmlChar *token,
			 void *data) {
    xmlRegAtomPtr atom;

    if ((am == NULL) || (from == NULL) || (token == NULL))
	return(NULL);
    atom = xmlRegNewAtom(am, XML_REGEXP_STRING);
    if (atom == NULL)
        return(NULL);
    atom->data = data;
    atom->valuep = xmlStrdup(token);

    if (xmlFAGenerateTransitions(am, from, to, atom) < 0) {
        xmlRegFreeAtom(atom);
	return(NULL);
    }
    if (to == NULL)
	return(am->state);
    return(to);
}