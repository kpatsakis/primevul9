xmlFAComputesDeterminism(xmlRegParserCtxtPtr ctxt) {
    int statenr, transnr;
    xmlRegStatePtr state;
    xmlRegTransPtr t1, t2, last;
    int i;
    int ret = 1;
    int deep = 1;

#ifdef DEBUG_REGEXP_GRAPH
    printf("xmlFAComputesDeterminism\n");
    xmlRegPrintCtxt(stdout, ctxt);
#endif
    if (ctxt->determinist != -1)
	return(ctxt->determinist);

    if (ctxt->flags & AM_AUTOMATA_RNG)
        deep = 0;

    /*
     * First cleanup the automata removing cancelled transitions
     */
    for (statenr = 0;statenr < ctxt->nbStates;statenr++) {
	state = ctxt->states[statenr];
	if (state == NULL)
	    continue;
	if (state->nbTrans < 2)
	    continue;
	for (transnr = 0;transnr < state->nbTrans;transnr++) {
	    t1 = &(state->trans[transnr]);
	    /*
	     * Determinism checks in case of counted or all transitions
	     * will have to be handled separately
	     */
	    if (t1->atom == NULL) {
		/* t1->nd = 1; */
		continue;
	    }
	    if (t1->to == -1) /* eliminated */
		continue;
	    for (i = 0;i < transnr;i++) {
		t2 = &(state->trans[i]);
		if (t2->to == -1) /* eliminated */
		    continue;
		if (t2->atom != NULL) {
		    if (t1->to == t2->to) {
                        /*
                         * Here we use deep because we want to keep the
                         * transitions which indicate a conflict
                         */
			if (xmlFAEqualAtoms(t1->atom, t2->atom, deep) &&
                            (t1->counter == t2->counter) &&
                            (t1->count == t2->count))
			    t2->to = -1; /* eliminated */
		    }
		}
	    }
	}
    }

    /*
     * Check for all states that there aren't 2 transitions
     * with the same atom and a different target.
     */
    for (statenr = 0;statenr < ctxt->nbStates;statenr++) {
	state = ctxt->states[statenr];
	if (state == NULL)
	    continue;
	if (state->nbTrans < 2)
	    continue;
	last = NULL;
	for (transnr = 0;transnr < state->nbTrans;transnr++) {
	    t1 = &(state->trans[transnr]);
	    /*
	     * Determinism checks in case of counted or all transitions
	     * will have to be handled separately
	     */
	    if (t1->atom == NULL) {
		continue;
	    }
	    if (t1->to == -1) /* eliminated */
		continue;
	    for (i = 0;i < transnr;i++) {
		t2 = &(state->trans[i]);
		if (t2->to == -1) /* eliminated */
		    continue;
		if (t2->atom != NULL) {
                    /*
                     * But here we don't use deep because we want to
                     * find transitions which indicate a conflict
                     */
		    if (xmlFACompareAtoms(t1->atom, t2->atom, 1)) {
			ret = 0;
			/* mark the transitions as non-deterministic ones */
			t1->nd = 1;
			t2->nd = 1;
			last = t1;
		    }
		} else if (t1->to != -1) {
		    /*
		     * do the closure in case of remaining specific
		     * epsilon transitions like choices or all
		     */
		    ret = xmlFARecurseDeterminism(ctxt, ctxt->states[t1->to],
						   t2->to, t2->atom);
		    /* don't shortcut the computation so all non deterministic
		       transition get marked down
		    if (ret == 0)
			return(0);
		     */
		    if (ret == 0) {
			t1->nd = 1;
			/* t2->nd = 1; */
			last = t1;
		    }
		}
	    }
	    /* don't shortcut the computation so all non deterministic
	       transition get marked down
	    if (ret == 0)
		break; */
	}

	/*
	 * mark specifically the last non-deterministic transition
	 * from a state since there is no need to set-up rollback
	 * from it
	 */
	if (last != NULL) {
	    last->nd = 2;
	}

	/* don't shortcut the computation so all non deterministic
	   transition get marked down
	if (ret == 0)
	    break; */
    }

    ctxt->determinist = ret;
    return(ret);
}