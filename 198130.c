xmlNewAutomata(void) {
    xmlAutomataPtr ctxt;

    ctxt = xmlRegNewParserCtxt(NULL);
    if (ctxt == NULL)
	return(NULL);

    /* initialize the parser */
    ctxt->end = NULL;
    ctxt->start = ctxt->state = xmlRegNewState(ctxt);
    if (ctxt->start == NULL) {
	xmlFreeAutomata(ctxt);
	return(NULL);
    }
    ctxt->start->type = XML_REGEXP_START_STATE;
    if (xmlRegStatePush(ctxt, ctxt->start) < 0) {
        xmlRegFreeState(ctxt->start);
	xmlFreeAutomata(ctxt);
	return(NULL);
    }
    ctxt->flags = 0;

    return(ctxt);
}