xmlFAParseBranch(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr to) {
    xmlRegStatePtr previous;
    int ret;

    previous = ctxt->state;
    ret = xmlFAParsePiece(ctxt);
    if (ret != 0) {
	if (xmlFAGenerateTransitions(ctxt, previous,
	        (CUR=='|' || CUR==')') ? to : NULL, ctxt->atom) < 0)
	    return(-1);
	previous = ctxt->state;
	ctxt->atom = NULL;
    }
    while ((ret != 0) && (ctxt->error == 0)) {
	ret = xmlFAParsePiece(ctxt);
	if (ret != 0) {
	    if (xmlFAGenerateTransitions(ctxt, previous,
	            (CUR=='|' || CUR==')') ? to : NULL, ctxt->atom) < 0)
		    return(-1);
	    previous = ctxt->state;
	    ctxt->atom = NULL;
	}
    }
    return(0);
}