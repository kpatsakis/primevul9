xmlXIncludeLoadFallback(xmlXIncludeCtxtPtr ctxt, xmlNodePtr fallback, int nr) {
    xmlXIncludeCtxtPtr newctxt;
    int ret = 0;
    int oldNbErrors = ctxt->nbErrors;

    if ((fallback == NULL) || (fallback->type == XML_NAMESPACE_DECL) ||
        (ctxt == NULL))
	return(-1);
    if (fallback->children != NULL) {
	/*
	 * It's possible that the fallback also has 'includes'
	 * (Bug 129969), so we re-process the fallback just in case
	 */
	newctxt = xmlXIncludeNewContext(ctxt->doc);
	if (newctxt == NULL)
	    return (-1);
	newctxt->_private = ctxt->_private;
	newctxt->base = xmlStrdup(ctxt->base);	/* Inherit the base from the existing context */
	xmlXIncludeSetFlags(newctxt, ctxt->parseFlags);
        newctxt->incTotal = ctxt->incTotal;
        if (xmlXIncludeDoProcess(newctxt, ctxt->doc, fallback, 1) < 0)
            ret = -1;
        ctxt->incTotal = newctxt->incTotal;
	if (ctxt->nbErrors > oldNbErrors)
	    ret = -1;
	xmlXIncludeFreeContext(newctxt);

	ctxt->incTab[nr]->inc = xmlDocCopyNodeList(ctxt->doc,
	                                           fallback->children);
        if (ctxt->incTab[nr]->inc == NULL)
            ctxt->incTab[nr]->emptyFb = 1;
    } else {
        ctxt->incTab[nr]->inc = NULL;
	ctxt->incTab[nr]->emptyFb = 1;	/* flag empty callback */
    }
    ctxt->incTab[nr]->fallback = 1;
    return(ret);
}