xmlFARecurseDeterminism(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr state,
	                 int to, xmlRegAtomPtr atom) {
    int ret = 1;
    int res;
    int transnr, nbTrans;
    xmlRegTransPtr t1;
    int deep = 1;

    if (state == NULL)
	return(ret);
    if (state->markd == XML_REGEXP_MARK_VISITED)
	return(ret);

    if (ctxt->flags & AM_AUTOMATA_RNG)
        deep = 0;

    /*
     * don't recurse on transitions potentially added in the course of
     * the elimination.
     */
    nbTrans = state->nbTrans;
    for (transnr = 0;transnr < nbTrans;transnr++) {
	t1 = &(state->trans[transnr]);
	/*
	 * check transitions conflicting with the one looked at
	 */
	if (t1->atom == NULL) {
	    if (t1->to < 0)
		continue;
	    state->markd = XML_REGEXP_MARK_VISITED;
	    res = xmlFARecurseDeterminism(ctxt, ctxt->states[t1->to],
		                           to, atom);
	    state->markd = 0;
	    if (res == 0) {
	        ret = 0;
		/* t1->nd = 1; */
	    }
	    continue;
	}
	if (t1->to != to)
	    continue;
	if (xmlFACompareAtoms(t1->atom, atom, deep)) {
	    ret = 0;
	    /* mark the transition as non-deterministic */
	    t1->nd = 1;
	}
    }
    return(ret);
}