xsltParseStylesheetStripSpace(xsltStylesheetPtr style, xmlNodePtr cur) {
    xmlChar *elements;
    xmlChar *element, *end;

    if ((cur == NULL) || (style == NULL))
	return;

    elements = xmlGetNsProp(cur, (const xmlChar *)"elements", NULL);
    if (elements == NULL) {
	xsltTransformError(NULL, style, cur,
	    "xsltParseStylesheetStripSpace: missing elements attribute\n");
	if (style != NULL) style->warnings++;
	return;
    }

    if (style->stripSpaces == NULL)
	style->stripSpaces = xmlHashCreate(10);
    if (style->stripSpaces == NULL)
	return;

    element = elements;
    while (*element != 0) {
	while (IS_BLANK(*element)) element++;
	if (*element == 0)
	    break;
        end = element;
	while ((*end != 0) && (!IS_BLANK(*end))) end++;
	element = xmlStrndup(element, end - element);
	if (element) {
#ifdef WITH_XSLT_DEBUG_PARSING
	    xsltGenericDebug(xsltGenericDebugContext,
		"add stripped space element %s\n", element);
#endif
	    if (xmlStrEqual(element, (const xmlChar *)"*")) {
		style->stripAll = 1;
	    } else {
		const xmlChar *URI;

		/*
		* TODO: Don't use xsltGetQNameURI().
		*/
                URI = xsltGetQNameURI(cur, &element);

		xmlHashAddEntry2(style->stripSpaces, element, URI,
			        (xmlChar *) "strip");
	    }
	    xmlFree(element);
	}
	element = end;
    }
    xmlFree(elements);
    if (cur->children != NULL) {
	xsltParseContentError(style, cur->children);
    }
}