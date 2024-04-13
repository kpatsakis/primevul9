xmlXIncludeAddNode(xmlXIncludeCtxtPtr ctxt, xmlNodePtr cur) {
    xmlXIncludeRefPtr ref;
    xmlURIPtr uri;
    xmlChar *URL;
    xmlChar *fragment = NULL;
    xmlChar *href;
    xmlChar *parse;
    xmlChar *base;
    xmlChar *URI;
    int xml = 1, i; /* default Issue 64 */
    int local = 0;


    if (ctxt == NULL)
	return(-1);
    if (cur == NULL)
	return(-1);

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Add node\n");
#endif
    /*
     * read the attributes
     */
    href = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_HREF);
    if (href == NULL) {
	href = xmlStrdup(BAD_CAST ""); /* @@@@ href is now optional */
	if (href == NULL)
	    return(-1);
    }
    if ((href[0] == '#') || (href[0] == 0))
	local = 1;
    parse = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_PARSE);
    if (parse != NULL) {
	if (xmlStrEqual(parse, XINCLUDE_PARSE_XML))
	    xml = 1;
	else if (xmlStrEqual(parse, XINCLUDE_PARSE_TEXT))
	    xml = 0;
	else {
	    xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_PARSE_VALUE,
	                   "invalid value %s for 'parse'\n", parse);
	    if (href != NULL)
		xmlFree(href);
	    if (parse != NULL)
		xmlFree(parse);
	    return(-1);
	}
    }

    /*
     * compute the URI
     */
    base = xmlNodeGetBase(ctxt->doc, cur);
    if (base == NULL) {
	URI = xmlBuildURI(href, ctxt->doc->URL);
    } else {
	URI = xmlBuildURI(href, base);
    }
    if (URI == NULL) {
	xmlChar *escbase;
	xmlChar *eschref;
	/*
	 * Some escaping may be needed
	 */
	escbase = xmlURIEscape(base);
	eschref = xmlURIEscape(href);
	URI = xmlBuildURI(eschref, escbase);
	if (escbase != NULL)
	    xmlFree(escbase);
	if (eschref != NULL)
	    xmlFree(eschref);
    }
    if (parse != NULL)
	xmlFree(parse);
    if (href != NULL)
	xmlFree(href);
    if (base != NULL)
	xmlFree(base);
    if (URI == NULL) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_HREF_URI,
	               "failed build URL\n", NULL);
	return(-1);
    }
    fragment = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_PARSE_XPOINTER);

    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)URI);
    if (uri == NULL) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_HREF_URI,
	               "invalid value URI %s\n", URI);
	if (fragment != NULL)
	    xmlFree(fragment);
	xmlFree(URI);
	return(-1);
    }

    if (uri->fragment != NULL) {
        if (ctxt->legacy != 0) {
	    if (fragment == NULL) {
		fragment = (xmlChar *) uri->fragment;
	    } else {
		xmlFree(uri->fragment);
	    }
	} else {
	    xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_FRAGMENT_ID,
       "Invalid fragment identifier in URI %s use the xpointer attribute\n",
                           URI);
	    if (fragment != NULL)
	        xmlFree(fragment);
	    xmlFreeURI(uri);
	    xmlFree(URI);
	    return(-1);
	}
	uri->fragment = NULL;
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    xmlFree(URI);
    if (URL == NULL) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_HREF_URI,
	               "invalid value URI %s\n", URI);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }

    /*
     * If local and xml then we need a fragment
     */
    if ((local == 1) && (xml == 1) &&
        ((fragment == NULL) || (fragment[0] == 0))) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_RECURSION,
	               "detected a local recursion with no xpointer in %s\n",
		       URL);
        xmlFree(URL);
        xmlFree(fragment);
	return(-1);
    }

    /*
     * Check the URL against the stack for recursions
     */
    if ((!local) && (xml == 1)) {
	for (i = 0;i < ctxt->urlNr;i++) {
	    if (xmlStrEqual(URL, ctxt->urlTab[i])) {
		xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_RECURSION,
		               "detected a recursion in %s\n", URL);
                xmlFree(URL);
                xmlFree(fragment);
		return(-1);
	    }
	}
    }

    ref = xmlXIncludeNewRef(ctxt, URL, cur);
    xmlFree(URL);
    if (ref == NULL) {
	return(-1);
    }
    ref->fragment = fragment;
    ref->doc = NULL;
    ref->xml = xml;
    ref->count = 1;
    return(0);
}