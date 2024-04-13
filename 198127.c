xmlFAReduceEpsilonTransitions(xmlRegParserCtxtPtr ctxt, int fromnr,
	                      int tonr, int counter) {
    int transnr;
    xmlRegStatePtr from;
    xmlRegStatePtr to;

#ifdef DEBUG_REGEXP_GRAPH
    printf("xmlFAReduceEpsilonTransitions(%d, %d)\n", fromnr, tonr);
#endif
    from = ctxt->states[fromnr];
    if (from == NULL)
	return;
    to = ctxt->states[tonr];
    if (to == NULL)
	return;
    if ((to->mark == XML_REGEXP_MARK_START) ||
	(to->mark == XML_REGEXP_MARK_VISITED))
	return;

    to->mark = XML_REGEXP_MARK_VISITED;
    if (to->type == XML_REGEXP_FINAL_STATE) {
#ifdef DEBUG_REGEXP_GRAPH
	printf("State %d is final, so %d becomes final\n", tonr, fromnr);
#endif
	from->type = XML_REGEXP_FINAL_STATE;
    }
    for (transnr = 0;transnr < to->nbTrans;transnr++) {
        if (to->trans[transnr].to < 0)
	    continue;
	if (to->trans[transnr].atom == NULL) {
	    /*
	     * Don't remove counted transitions
	     * Don't loop either
	     */
	    if (to->trans[transnr].to != fromnr) {
		if (to->trans[transnr].count >= 0) {
		    int newto = to->trans[transnr].to;

		    xmlRegStateAddTrans(ctxt, from, NULL,
					ctxt->states[newto],
					-1, to->trans[transnr].count);
		} else {
#ifdef DEBUG_REGEXP_GRAPH
		    printf("Found epsilon trans %d from %d to %d\n",
			   transnr, tonr, to->trans[transnr].to);
#endif
		    if (to->trans[transnr].counter >= 0) {
			xmlFAReduceEpsilonTransitions(ctxt, fromnr,
					      to->trans[transnr].to,
					      to->trans[transnr].counter);
		    } else {
			xmlFAReduceEpsilonTransitions(ctxt, fromnr,
					      to->trans[transnr].to,
					      counter);
		    }
		}
	    }
	} else {
	    int newto = to->trans[transnr].to;

	    if (to->trans[transnr].counter >= 0) {
		xmlRegStateAddTrans(ctxt, from, to->trans[transnr].atom,
				    ctxt->states[newto],
				    to->trans[transnr].counter, -1);
	    } else {
		xmlRegStateAddTrans(ctxt, from, to->trans[transnr].atom,
				    ctxt->states[newto], counter, -1);
	    }
	}
    }
    to->mark = XML_REGEXP_MARK_NORMAL;
}