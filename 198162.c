xmlRegStateAddTrans(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr state,
	            xmlRegAtomPtr atom, xmlRegStatePtr target,
		    int counter, int count) {

    int nrtrans;

    if (state == NULL) {
	ERROR("add state: state is NULL");
	return;
    }
    if (target == NULL) {
	ERROR("add state: target is NULL");
	return;
    }
    /*
     * Other routines follow the philosophy 'When in doubt, add a transition'
     * so we check here whether such a transition is already present and, if
     * so, silently ignore this request.
     */

    for (nrtrans = state->nbTrans - 1; nrtrans >= 0; nrtrans--) {
	xmlRegTransPtr trans = &(state->trans[nrtrans]);
	if ((trans->atom == atom) &&
	    (trans->to == target->no) &&
	    (trans->counter == counter) &&
	    (trans->count == count)) {
#ifdef DEBUG_REGEXP_GRAPH
	    printf("Ignoring duplicate transition from %d to %d\n",
		    state->no, target->no);
#endif
	    return;
	}
    }

    if (state->maxTrans == 0) {
	state->maxTrans = 8;
	state->trans = (xmlRegTrans *) xmlMalloc(state->maxTrans *
		                             sizeof(xmlRegTrans));
	if (state->trans == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding transition");
	    state->maxTrans = 0;
	    return;
	}
    } else if (state->nbTrans >= state->maxTrans) {
	xmlRegTrans *tmp;
	state->maxTrans *= 2;
	tmp = (xmlRegTrans *) xmlRealloc(state->trans, state->maxTrans *
		                             sizeof(xmlRegTrans));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding transition");
	    state->maxTrans /= 2;
	    return;
	}
	state->trans = tmp;
    }
#ifdef DEBUG_REGEXP_GRAPH
    printf("Add trans from %d to %d ", state->no, target->no);
    if (count == REGEXP_ALL_COUNTER)
	printf("all transition\n");
    else if (count >= 0)
	printf("count based %d\n", count);
    else if (counter >= 0)
	printf("counted %d\n", counter);
    else if (atom == NULL)
	printf("epsilon transition\n");
    else if (atom != NULL)
        xmlRegPrintAtom(stdout, atom);
#endif

    state->trans[state->nbTrans].atom = atom;
    state->trans[state->nbTrans].to = target->no;
    state->trans[state->nbTrans].counter = counter;
    state->trans[state->nbTrans].count = count;
    state->trans[state->nbTrans].nd = 0;
    state->nbTrans++;
    xmlRegStateAddTransTo(ctxt, target, state->no);
}