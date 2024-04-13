xmlXIncludeRecurseDoc(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc,
	              const xmlURL url ATTRIBUTE_UNUSED) {
    xmlXIncludeCtxtPtr newctxt;
    int i;

    /*
     * Avoid recursion in already substituted resources
    for (i = 0;i < ctxt->urlNr;i++) {
	if (xmlStrEqual(doc->URL, ctxt->urlTab[i]))
	    return;
    }
     */

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Recursing in doc %s\n", doc->URL);
#endif
    /*
     * Handle recursion here.
     */

    newctxt = xmlXIncludeNewContext(doc);
    if (newctxt != NULL) {
	/*
	 * Copy the private user data
	 */
	newctxt->_private = ctxt->_private;
	/*
	 * Copy the existing document set
	 */
	newctxt->incMax = ctxt->incMax;
	newctxt->incNr = ctxt->incNr;
        newctxt->incTab = (xmlXIncludeRefPtr *) xmlMalloc(newctxt->incMax *
		                          sizeof(newctxt->incTab[0]));
        if (newctxt->incTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, (xmlNodePtr) doc, "processing doc");
	    xmlFree(newctxt);
	    return;
	}
	/*
	 * copy the urlTab
	 */
	newctxt->urlMax = ctxt->urlMax;
	newctxt->urlNr = ctxt->urlNr;
	newctxt->urlTab = ctxt->urlTab;

	/*
	 * Inherit the existing base
	 */
	newctxt->base = xmlStrdup(ctxt->base);

	/*
	 * Inherit the documents already in use by other includes
	 */
	newctxt->incBase = ctxt->incNr;
	for (i = 0;i < ctxt->incNr;i++) {
	    newctxt->incTab[i] = ctxt->incTab[i];
	    newctxt->incTab[i]->count++; /* prevent the recursion from
					    freeing it */
	}
	/*
	 * The new context should also inherit the Parse Flags
	 * (bug 132597)
	 */
	newctxt->parseFlags = ctxt->parseFlags;
        newctxt->incTotal = ctxt->incTotal;
	xmlXIncludeDoProcess(newctxt, doc, xmlDocGetRootElement(doc), 0);
        ctxt->incTotal = newctxt->incTotal;
	for (i = 0;i < ctxt->incNr;i++) {
	    newctxt->incTab[i]->count--;
	    newctxt->incTab[i] = NULL;
	}

	/* urlTab may have been reallocated */
	ctxt->urlTab = newctxt->urlTab;
	ctxt->urlMax = newctxt->urlMax;

	newctxt->urlMax = 0;
	newctxt->urlNr = 0;
	newctxt->urlTab = NULL;

	xmlXIncludeFreeContext(newctxt);
    }
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Done recursing in doc %s\n", url);
#endif
}