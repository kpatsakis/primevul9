xmlFAEliminateSimpleEpsilonTransitions(xmlRegParserCtxtPtr ctxt) {
    int statenr, i, j, newto;
    xmlRegStatePtr state, tmp;

    for (statenr = 0;statenr < ctxt->nbStates;statenr++) {
	state = ctxt->states[statenr];
	if (state == NULL)
	    continue;
	if (state->nbTrans != 1)
	    continue;
	if (state->type == XML_REGEXP_UNREACH_STATE)
	    continue;
	/* is the only transition out a basic transition */
	if ((state->trans[0].atom == NULL) &&
	    (state->trans[0].to >= 0) &&
	    (state->trans[0].to != statenr) &&
	    (state->trans[0].counter < 0) &&
	    (state->trans[0].count < 0)) {
	    newto = state->trans[0].to;

            if (state->type == XML_REGEXP_START_STATE) {
#ifdef DEBUG_REGEXP_GRAPH
		printf("Found simple epsilon trans from start %d to %d\n",
		       statenr, newto);
#endif
            } else {
#ifdef DEBUG_REGEXP_GRAPH
		printf("Found simple epsilon trans from %d to %d\n",
		       statenr, newto);
#endif
	        for (i = 0;i < state->nbTransTo;i++) {
		    tmp = ctxt->states[state->transTo[i]];
		    for (j = 0;j < tmp->nbTrans;j++) {
			if (tmp->trans[j].to == statenr) {
#ifdef DEBUG_REGEXP_GRAPH
			    printf("Changed transition %d on %d to go to %d\n",
				   j, tmp->no, newto);
#endif
			    tmp->trans[j].to = -1;
			    xmlRegStateAddTrans(ctxt, tmp, tmp->trans[j].atom,
						ctxt->states[newto],
					        tmp->trans[j].counter,
						tmp->trans[j].count);
			}
		    }
		}
		if (state->type == XML_REGEXP_FINAL_STATE)
		    ctxt->states[newto]->type = XML_REGEXP_FINAL_STATE;
		/* eliminate the transition completely */
		state->nbTrans = 0;

                state->type = XML_REGEXP_UNREACH_STATE;

	    }

	}
    }
}