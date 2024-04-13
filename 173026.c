xmlIOParseDTD(xmlSAXHandlerPtr sax, xmlParserInputBufferPtr input,
	      xmlCharEncoding enc) {
    xmlDtdPtr ret = NULL;
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr pinput = NULL;
    xmlChar start[4];

    if (input == NULL)
	return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        xmlFreeParserInputBuffer(input);
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
    xmlDetectSAX2(ctxt);

    /*
     * generate a parser input from the I/O handler
     */

    pinput = xmlNewIOInputStream(ctxt, input, XML_CHAR_ENCODING_NONE);
    if (pinput == NULL) {
        if (sax != NULL) ctxt->sax = NULL;
        xmlFreeParserInputBuffer(input);
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    /*
     * plug some encoding conversion routines here.
     */
    if (xmlPushInput(ctxt, pinput) < 0) {
        if (sax != NULL) ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    pinput->filename = NULL;
    pinput->line = 1;
    pinput->col = 1;
    pinput->base = ctxt->input->cur;
    pinput->cur = ctxt->input->cur;
    pinput->free = NULL;

    /*
     * let's parse that entity knowing it's an external subset.
     */
    ctxt->inSubset = 2;
    ctxt->myDoc = xmlNewDoc(BAD_CAST "1.0");
    if (ctxt->myDoc == NULL) {
	xmlErrMemory(ctxt, "New Doc failed");
	return(NULL);
    }
    ctxt->myDoc->properties = XML_DOC_INTERNAL;
    ctxt->myDoc->extSubset = xmlNewDtd(ctxt->myDoc, BAD_CAST "none",
	                               BAD_CAST "none", BAD_CAST "none");

    if ((enc == XML_CHAR_ENCODING_NONE) &&
        ((ctxt->input->end - ctxt->input->cur) >= 4)) {
	/*
	 * Get the 4 first bytes and decode the charset
	 * if enc != XML_CHAR_ENCODING_NONE
	 * plug some encoding conversion routines.
	 */
	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE) {
	    xmlSwitchEncoding(ctxt, enc);
	}
    }

    xmlParseExternalSubset(ctxt, BAD_CAST "none", BAD_CAST "none");

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