xmlXIncludeLoadNode(xmlXIncludeCtxtPtr ctxt, int nr) {
    xmlNodePtr cur;
    xmlChar *href;
    xmlChar *parse;
    xmlChar *base;
    xmlChar *oldBase;
    xmlChar *URI;
    int xml = 1; /* default Issue 64 */
    int ret;

    if (ctxt == NULL)
	return(-1);
    if ((nr < 0) || (nr >= ctxt->incNr))
	return(-1);
    cur = ctxt->incTab[nr]->ref;
    if (cur == NULL)
	return(-1);

    /*
     * read the attributes
     */
    href = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_HREF);
    if (href == NULL) {
	href = xmlStrdup(BAD_CAST ""); /* @@@@ href is now optional */
	if (href == NULL)
	    return(-1);
    }
    parse = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_PARSE);
    if (parse != NULL) {
	if (xmlStrEqual(parse, XINCLUDE_PARSE_XML))
	    xml = 1;
	else if (xmlStrEqual(parse, XINCLUDE_PARSE_TEXT))
	    xml = 0;
	else {
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	                   XML_XINCLUDE_PARSE_VALUE,
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
    if (URI == NULL) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	               XML_XINCLUDE_HREF_URI, "failed build URL\n", NULL);
	if (parse != NULL)
	    xmlFree(parse);
	if (href != NULL)
	    xmlFree(href);
	if (base != NULL)
	    xmlFree(base);
	return(-1);
    }
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "parse: %s\n",
	    xml ? "xml": "text");
    xmlGenericError(xmlGenericErrorContext, "URI: %s\n", URI);
#endif

    /*
     * Save the base for this include (saving the current one)
     */
    oldBase = ctxt->base;
    ctxt->base = base;

    if (xml) {
	ret = xmlXIncludeLoadDoc(ctxt, URI, nr);
	/* xmlXIncludeGetFragment(ctxt, cur, URI); */
    } else {
	ret = xmlXIncludeLoadTxt(ctxt, URI, nr);
    }

    /*
     * Restore the original base before checking for fallback
     */
    ctxt->base = oldBase;

    if (ret < 0) {
	xmlNodePtr children;

	/*
	 * Time to try a fallback if available
	 */
#ifdef DEBUG_XINCLUDE
	xmlGenericError(xmlGenericErrorContext, "error looking for fallback\n");
#endif
	children = cur->children;
	while (children != NULL) {
	    if ((children->type == XML_ELEMENT_NODE) &&
		(children->ns != NULL) &&
		(xmlStrEqual(children->name, XINCLUDE_FALLBACK)) &&
		((xmlStrEqual(children->ns->href, XINCLUDE_NS)) ||
		 (xmlStrEqual(children->ns->href, XINCLUDE_OLD_NS)))) {
		ret = xmlXIncludeLoadFallback(ctxt, children, nr);
		break;
	    }
	    children = children->next;
	}
    }
    if (ret < 0) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	               XML_XINCLUDE_NO_FALLBACK,
		       "could not load %s, and no fallback was found\n",
		       URI);
    }

    /*
     * Cleanup
     */
    if (URI != NULL)
	xmlFree(URI);
    if (parse != NULL)
	xmlFree(parse);
    if (href != NULL)
	xmlFree(href);
    if (base != NULL)
	xmlFree(base);
    return(0);
}