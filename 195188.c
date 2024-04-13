xmlXIncludeParseFile(xmlXIncludeCtxtPtr ctxt, const char *URL) {
    xmlDocPtr ret;
    xmlParserCtxtPtr pctxt;
    xmlParserInputPtr inputStream;

    xmlInitParser();

    pctxt = xmlNewParserCtxt();
    if (pctxt == NULL) {
	xmlXIncludeErrMemory(ctxt, NULL, "cannot allocate parser context");
	return(NULL);
    }

    /*
     * pass in the application data to the parser context.
     */
    pctxt->_private = ctxt->_private;

    /*
     * try to ensure that new documents included are actually
     * built with the same dictionary as the including document.
     */
    if ((ctxt->doc != NULL) && (ctxt->doc->dict != NULL)) {
       if (pctxt->dict != NULL)
            xmlDictFree(pctxt->dict);
	pctxt->dict = ctxt->doc->dict;
	xmlDictReference(pctxt->dict);
    }

    xmlCtxtUseOptions(pctxt, ctxt->parseFlags | XML_PARSE_DTDLOAD);

    /* Don't read from stdin. */
    if ((URL != NULL) && (strcmp(URL, "-") == 0))
        URL = "./-";

    inputStream = xmlLoadExternalEntity(URL, NULL, pctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(pctxt);
	return(NULL);
    }

    inputPush(pctxt, inputStream);

    if (pctxt->directory == NULL)
        pctxt->directory = xmlParserGetDirectory(URL);

    pctxt->loadsubset |= XML_DETECT_IDS;

    xmlParseDocument(pctxt);

    if (pctxt->wellFormed) {
        ret = pctxt->myDoc;
    }
    else {
        ret = NULL;
	if (pctxt->myDoc != NULL)
	    xmlFreeDoc(pctxt->myDoc);
        pctxt->myDoc = NULL;
    }
    xmlFreeParserCtxt(pctxt);

    return(ret);
}