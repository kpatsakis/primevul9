xmlSAXParseDTD(xmlSAXHandlerPtr sax, const xmlChar *ExternalID,
                          const xmlChar *SystemID) {
    xmlDtdPtr ret = NULL;
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr input = NULL;
    xmlCharEncoding enc;
    xmlChar* systemIdCanonic;

    if ((ExternalID == NULL) && (SystemID == NULL)) return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	return(NULL);
    }

    /*
     * Set-up the SAX context
     */
    if (sax != NULL) {
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
        ctxt->userData = ctxt;
    }

    /*
     * Canonicalise the system ID
     */
    systemIdCanonic = xmlCanonicPath(SystemID);
    if ((SystemID != NULL) && (systemIdCanonic == NULL)) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    /*
     * Ask the Entity resolver to load the damn thing
     */

    if ((ctxt->sax != NULL) && (ctxt->sax->resolveEntity != NULL))
	input = ctxt->sax->resolveEntity(ctxt->userData, ExternalID,
	                                 systemIdCanonic);
    if (input == NULL) {
        if (sax != NULL) ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	if (systemIdCanonic != NULL)
	    xmlFree(systemIdCanonic);
	return(NULL);
    }

    /*
     * plug some encoding conversion routines here.
     */
    if (xmlPushInput(ctxt, input) < 0) {
        if (sax != NULL) ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	if (systemIdCanonic != NULL)
	    xmlFree(systemIdCanonic);
	return(NULL);
    }
    if ((ctxt->input->end - ctxt->input->cur) >= 4) {
	enc = xmlDetectCharEncoding(ctxt->input->cur, 4);
	xmlSwitchEncoding(ctxt, enc);
    }

    if (input->filename == NULL)
	input->filename = (char *) systemIdCanonic;
    else
	xmlFree(systemIdCanonic);
    input->line = 1;
    input->col = 1;
    input->base = ctxt->input->cur;
    input->cur = ctxt->input->cur;
    input->free = NULL;

    /*
     * let's parse that entity knowing it's an external subset.
     */
    ctxt->inSubset = 2;
    ctxt->myDoc = xmlNewDoc(BAD_CAST "1.0");
    if (ctxt->myDoc == NULL) {
	xmlErrMemory(ctxt, "New Doc failed");
        if (sax != NULL) ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    ctxt->myDoc->properties = XML_DOC_INTERNAL;
    ctxt->myDoc->extSubset = xmlNewDtd(ctxt->myDoc, BAD_CAST "none",
	                               ExternalID, SystemID);
    xmlParseExternalSubset(ctxt, ExternalID, SystemID);

    if (ctxt->myDoc != NULL) {
	if (ctxt->wellFormed) {
	    ret = ctxt->myDoc->extSubset;
	    ctxt->myDoc->extSubset = NULL;
	    if (ret != NULL) {
		xmlNodePtr tmp;

		ret->doc = NULL;
		tmp = ret->children;
		while (tmp != NULL) {
		    tmp->doc = NULL;
		    tmp = tmp->next;
		}
	    }
	} else {
	    ret = NULL;
	}
        xmlFreeDoc(ctxt->myDoc);
        ctxt->myDoc = NULL;
    }
    if (sax != NULL) ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);

    return(ret);
}