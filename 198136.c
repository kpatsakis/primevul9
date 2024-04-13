xmlRegStatePush(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr state) {
    if (state == NULL) return(-1);
    if (ctxt->maxStates == 0) {
	ctxt->maxStates = 4;
	ctxt->states = (xmlRegStatePtr *) xmlMalloc(ctxt->maxStates *
		                             sizeof(xmlRegStatePtr));
	if (ctxt->states == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding state");
	    ctxt->maxStates = 0;
	    return(-1);
	}
    } else if (ctxt->nbStates >= ctxt->maxStates) {
	xmlRegStatePtr *tmp;
	ctxt->maxStates *= 2;
	tmp = (xmlRegStatePtr *) xmlRealloc(ctxt->states, ctxt->maxStates *
		                             sizeof(xmlRegStatePtr));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding state");
	    ctxt->maxStates /= 2;
	    return(-1);
	}
	ctxt->states = tmp;
    }
    state->no = ctxt->nbStates;
    ctxt->states[ctxt->nbStates++] = state;
    return(0);
}