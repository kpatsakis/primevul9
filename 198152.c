xmlFAGenerateTransitions(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr from,
	                 xmlRegStatePtr to, xmlRegAtomPtr atom) {
    xmlRegStatePtr end;
    int nullable = 0;

    if (atom == NULL) {
	ERROR("genrate transition: atom == NULL");
	return(-1);
    }
    if (atom->type == XML_REGEXP_SUBREG) {
	/*
	 * this is a subexpression handling one should not need to
	 * create a new node except for XML_REGEXP_QUANT_RANGE.
	 */
	if (xmlRegAtomPush(ctxt, atom) < 0) {
	    return(-1);
	}
	if ((to != NULL) && (atom->stop != to) &&
	    (atom->quant != XML_REGEXP_QUANT_RANGE)) {
	    /*
	     * Generate an epsilon transition to link to the target
	     */
	    xmlFAGenerateEpsilonTransition(ctxt, atom->stop, to);
#ifdef DV
	} else if ((to == NULL) && (atom->quant != XML_REGEXP_QUANT_RANGE) &&
		   (atom->quant != XML_REGEXP_QUANT_ONCE)) {
	    to = xmlRegNewState(ctxt);
	    xmlRegStatePush(ctxt, to);
	    ctxt->state = to;
	    xmlFAGenerateEpsilonTransition(ctxt, atom->stop, to);
#endif
	}
	switch (atom->quant) {
	    case XML_REGEXP_QUANT_OPT:
		atom->quant = XML_REGEXP_QUANT_ONCE;
		/*
		 * transition done to the state after end of atom.
		 *      1. set transition from atom start to new state
		 *      2. set transition from atom end to this state.
		 */
                if (to == NULL) {
                    xmlFAGenerateEpsilonTransition(ctxt, atom->start, 0);
                    xmlFAGenerateEpsilonTransition(ctxt, atom->stop,
                                                   ctxt->state);
                } else {
                    xmlFAGenerateEpsilonTransition(ctxt, atom->start, to);
                }
		break;
	    case XML_REGEXP_QUANT_MULT:
		atom->quant = XML_REGEXP_QUANT_ONCE;
		xmlFAGenerateEpsilonTransition(ctxt, atom->start, atom->stop);
		xmlFAGenerateEpsilonTransition(ctxt, atom->stop, atom->start);
		break;
	    case XML_REGEXP_QUANT_PLUS:
		atom->quant = XML_REGEXP_QUANT_ONCE;
		xmlFAGenerateEpsilonTransition(ctxt, atom->stop, atom->start);
		break;
	    case XML_REGEXP_QUANT_RANGE: {
		int counter;
		xmlRegStatePtr inter, newstate;

		/*
		 * create the final state now if needed
		 */
		if (to != NULL) {
		    newstate = to;
		} else {
		    newstate = xmlRegNewState(ctxt);
		    xmlRegStatePush(ctxt, newstate);
		}

		/*
		 * The principle here is to use counted transition
		 * to avoid explosion in the number of states in the
		 * graph. This is clearly more complex but should not
		 * be exploitable at runtime.
		 */
		if ((atom->min == 0) && (atom->start0 == NULL)) {
		    xmlRegAtomPtr copy;
		    /*
		     * duplicate a transition based on atom to count next
		     * occurences after 1. We cannot loop to atom->start
		     * directly because we need an epsilon transition to
		     * newstate.
		     */
		     /* ???? For some reason it seems we never reach that
		        case, I suppose this got optimized out before when
			building the automata */
		    copy = xmlRegCopyAtom(ctxt, atom);
		    if (copy == NULL)
		        return(-1);
		    copy->quant = XML_REGEXP_QUANT_ONCE;
		    copy->min = 0;
		    copy->max = 0;

		    if (xmlFAGenerateTransitions(ctxt, atom->start, NULL, copy)
		        < 0)
			return(-1);
		    inter = ctxt->state;
		    counter = xmlRegGetCounter(ctxt);
		    ctxt->counters[counter].min = atom->min - 1;
		    ctxt->counters[counter].max = atom->max - 1;
		    /* count the number of times we see it again */
		    xmlFAGenerateCountedEpsilonTransition(ctxt, inter,
						   atom->stop, counter);
		    /* allow a way out based on the count */
		    xmlFAGenerateCountedTransition(ctxt, inter,
			                           newstate, counter);
		    /* and also allow a direct exit for 0 */
		    xmlFAGenerateEpsilonTransition(ctxt, atom->start,
		                                   newstate);
		} else {
		    /*
		     * either we need the atom at least once or there
		     * is an atom->start0 allowing to easilly plug the
		     * epsilon transition.
		     */
		    counter = xmlRegGetCounter(ctxt);
		    ctxt->counters[counter].min = atom->min - 1;
		    ctxt->counters[counter].max = atom->max - 1;
		    /* count the number of times we see it again */
		    xmlFAGenerateCountedEpsilonTransition(ctxt, atom->stop,
						   atom->start, counter);
		    /* allow a way out based on the count */
		    xmlFAGenerateCountedTransition(ctxt, atom->stop,
			                           newstate, counter);
		    /* and if needed allow a direct exit for 0 */
		    if (atom->min == 0)
			xmlFAGenerateEpsilonTransition(ctxt, atom->start0,
						       newstate);

		}
		atom->min = 0;
		atom->max = 0;
		atom->quant = XML_REGEXP_QUANT_ONCE;
		ctxt->state = newstate;
	    }
	    default:
		break;
	}
	return(0);
    }
    if ((atom->min == 0) && (atom->max == 0) &&
               (atom->quant == XML_REGEXP_QUANT_RANGE)) {
        /*
	 * we can discard the atom and generate an epsilon transition instead
	 */
	if (to == NULL) {
	    to = xmlRegNewState(ctxt);
	    if (to != NULL)
		xmlRegStatePush(ctxt, to);
	    else {
		return(-1);
	    }
	}
	xmlFAGenerateEpsilonTransition(ctxt, from, to);
	ctxt->state = to;
	xmlRegFreeAtom(atom);
	return(0);
    }
    if (to == NULL) {
	to = xmlRegNewState(ctxt);
	if (to != NULL)
	    xmlRegStatePush(ctxt, to);
	else {
	    return(-1);
	}
    }
    end = to;
    if ((atom->quant == XML_REGEXP_QUANT_MULT) ||
        (atom->quant == XML_REGEXP_QUANT_PLUS)) {
	/*
	 * Do not pollute the target state by adding transitions from
	 * it as it is likely to be the shared target of multiple branches.
	 * So isolate with an epsilon transition.
	 */
        xmlRegStatePtr tmp;

	tmp = xmlRegNewState(ctxt);
	if (tmp != NULL)
	    xmlRegStatePush(ctxt, tmp);
	else {
	    return(-1);
	}
	xmlFAGenerateEpsilonTransition(ctxt, tmp, to);
	to = tmp;
    }
    if (xmlRegAtomPush(ctxt, atom) < 0) {
	return(-1);
    }
    if ((atom->quant == XML_REGEXP_QUANT_RANGE) &&
        (atom->min == 0) && (atom->max > 0)) {
	nullable = 1;
	atom->min = 1;
        if (atom->max == 1)
	    atom->quant = XML_REGEXP_QUANT_OPT;
    }
    xmlRegStateAddTrans(ctxt, from, atom, to, -1, -1);
    ctxt->state = end;
    switch (atom->quant) {
	case XML_REGEXP_QUANT_OPT:
	    atom->quant = XML_REGEXP_QUANT_ONCE;
	    xmlFAGenerateEpsilonTransition(ctxt, from, to);
	    break;
	case XML_REGEXP_QUANT_MULT:
	    atom->quant = XML_REGEXP_QUANT_ONCE;
	    xmlFAGenerateEpsilonTransition(ctxt, from, to);
	    xmlRegStateAddTrans(ctxt, to, atom, to, -1, -1);
	    break;
	case XML_REGEXP_QUANT_PLUS:
	    atom->quant = XML_REGEXP_QUANT_ONCE;
	    xmlRegStateAddTrans(ctxt, to, atom, to, -1, -1);
	    break;
	case XML_REGEXP_QUANT_RANGE:
	    if (nullable)
		xmlFAGenerateEpsilonTransition(ctxt, from, to);
	    break;
	default:
	    break;
    }
    return(0);
}