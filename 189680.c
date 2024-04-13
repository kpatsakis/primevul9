xsltPatPushState(xsltTransformContextPtr ctxt, xsltStepStates *states,
                 int step, xmlNodePtr node) {
    if ((states->states == NULL) || (states->maxstates <= 0)) {
        states->maxstates = 4;
	states->nbstates = 0;
	states->states = xmlMalloc(4 * sizeof(xsltStepState));
    }
    else if (states->maxstates <= states->nbstates) {
        xsltStepState *tmp;

	tmp = (xsltStepStatePtr) xmlRealloc(states->states,
			       2 * states->maxstates * sizeof(xsltStepState));
	if (tmp == NULL) {
	    xsltGenericError(xsltGenericErrorContext,
	     "xsltPatPushState: memory re-allocation failure.\n");
	    ctxt->state = XSLT_STATE_STOPPED;
	    return(-1);
	}
	states->states = tmp;
	states->maxstates *= 2;
    }
    states->states[states->nbstates].step = step;
    states->states[states->nbstates++].node = node;
#if 0
    fprintf(stderr, "Push: %d, %s\n", step, node->name);
#endif
    return(0);
}