xmlRegexpCompile(const xmlChar *regexp) {
    xmlRegexpPtr ret;
    xmlRegParserCtxtPtr ctxt;

    ctxt = xmlRegNewParserCtxt(regexp);
    if (ctxt == NULL)
	return(NULL);

    /* initialize the parser */
    ctxt->end = NULL;
    ctxt->start = ctxt->state = xmlRegNewState(ctxt);
    xmlRegStatePush(ctxt, ctxt->start);

    /* parse the expression building an automata */
    xmlFAParseRegExp(ctxt, 1);
    if (CUR != 0) {
	ERROR("xmlFAParseRegExp: extra characters");
    }
    if (ctxt->error != 0) {
	xmlRegFreeParserCtxt(ctxt);
	return(NULL);
    }
    ctxt->end = ctxt->state;
    ctxt->start->type = XML_REGEXP_START_STATE;
    ctxt->end->type = XML_REGEXP_FINAL_STATE;

    /* remove the Epsilon except for counted transitions */
    xmlFAEliminateEpsilonTransitions(ctxt);


    if (ctxt->error != 0) {
	xmlRegFreeParserCtxt(ctxt);
	return(NULL);
    }
    ret = xmlRegEpxFromParse(ctxt);
    xmlRegFreeParserCtxt(ctxt);
    return(ret);
}