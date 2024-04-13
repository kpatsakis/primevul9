xmlParseNotationDecl(xmlParserCtxtPtr ctxt) {
    const xmlChar *name;
    xmlChar *Pubid;
    xmlChar *Systemid;

    if (CMP10(CUR_PTR, '<', '!', 'N', 'O', 'T', 'A', 'T', 'I', 'O', 'N')) {
	xmlParserInputPtr input = ctxt->input;
	SHRINK;
	SKIP(10);
	if (!IS_BLANK_CH(CUR)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			   "Space required after '<!NOTATION'\n");
	    return;
	}
	SKIP_BLANKS;

        name = xmlParseName(ctxt);
	if (name == NULL) {
	    xmlFatalErr(ctxt, XML_ERR_NOTATION_NOT_STARTED, NULL);
	    return;
	}
	if (!IS_BLANK_CH(CUR)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
		     "Space required after the NOTATION name'\n");
	    return;
	}
	if (xmlStrchr(name, ':') != NULL) {
	    xmlNsErr(ctxt, XML_NS_ERR_COLON,
		     "colon are forbidden from notation names '%s'\n",
		     name, NULL, NULL);
	}
	SKIP_BLANKS;

	/*
	 * Parse the IDs.
	 */
	Systemid = xmlParseExternalID(ctxt, &Pubid, 0);
	SKIP_BLANKS;

	if (RAW == '>') {
	    if (input != ctxt->input) {
		xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
	"Notation declaration doesn't start and stop in the same entity\n");
	    }
	    NEXT;
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		(ctxt->sax->notationDecl != NULL))
		ctxt->sax->notationDecl(ctxt->userData, name, Pubid, Systemid);
	} else {
	    xmlFatalErr(ctxt, XML_ERR_NOTATION_NOT_FINISHED, NULL);
	}
	if (Systemid != NULL) xmlFree(Systemid);
	if (Pubid != NULL) xmlFree(Pubid);
    }
}