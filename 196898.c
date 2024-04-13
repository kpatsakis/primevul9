static void walkDoc(xmlDocPtr doc) {
    xmlTextReaderPtr reader;
    int ret;

#ifdef LIBXML_PATTERN_ENABLED
    xmlNodePtr root;
    const xmlChar *namespaces[22];
    int i;
    xmlNsPtr ns;

    root = xmlDocGetRootElement(doc);
    if (root == NULL ) {
        xmlGenericError(xmlGenericErrorContext,
                "Document does not have a root element");
        progresult = XMLLINT_ERR_UNCLASS;
        return;
    }
    for (ns = root->nsDef, i = 0;ns != NULL && i < 20;ns=ns->next) {
        namespaces[i++] = ns->href;
        namespaces[i++] = ns->prefix;
    }
    namespaces[i++] = NULL;
    namespaces[i] = NULL;

    if (pattern != NULL) {
        patternc = xmlPatterncompile((const xmlChar *) pattern, doc->dict,
	                             0, &namespaces[0]);
	if (patternc == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "Pattern %s failed to compile\n", pattern);
            progresult = XMLLINT_ERR_SCHEMAPAT;
	    pattern = NULL;
	}
    }
    if (patternc != NULL) {
        patstream = xmlPatternGetStreamCtxt(patternc);
	if (patstream != NULL) {
	    ret = xmlStreamPush(patstream, NULL, NULL);
	    if (ret < 0) {
		fprintf(stderr, "xmlStreamPush() failure\n");
		xmlFreeStreamCtxt(patstream);
		patstream = NULL;
            }
	}
    }
#endif /* LIBXML_PATTERN_ENABLED */
    reader = xmlReaderWalker(doc);
    if (reader != NULL) {
	if ((timing) && (!repeat)) {
	    startTimer();
	}
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
	    if ((debug)
#ifdef LIBXML_PATTERN_ENABLED
	        || (patternc)
#endif
	       )
		processNode(reader);
	    ret = xmlTextReaderRead(reader);
	}
	if ((timing) && (!repeat)) {
	    endTimer("walking through the doc");
	}
	xmlFreeTextReader(reader);
	if (ret != 0) {
	    fprintf(stderr, "failed to walk through the doc\n");
	    progresult = XMLLINT_ERR_UNCLASS;
	}
    } else {
	fprintf(stderr, "Failed to crate a reader from the document\n");
	progresult = XMLLINT_ERR_UNCLASS;
    }
#ifdef LIBXML_PATTERN_ENABLED
    if (patstream != NULL) {
	xmlFreeStreamCtxt(patstream);
	patstream = NULL;
    }
#endif
}