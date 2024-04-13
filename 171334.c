xsltParseStylesheetExtPrefix(xsltStylesheetPtr style, xmlNodePtr cur,
			     int isXsltElem) {
    xmlChar *prefixes;
    xmlChar *prefix, *end;

    if ((cur == NULL) || (style == NULL))
	return;

    if (isXsltElem) {
	/* For xsl:stylesheet/xsl:transform. */
	prefixes = xmlGetNsProp(cur,
	    (const xmlChar *)"extension-element-prefixes", NULL);
    } else {
	/* For literal result elements and extension instructions. */
	prefixes = xmlGetNsProp(cur,
	    (const xmlChar *)"extension-element-prefixes", XSLT_NAMESPACE);
    }
    if (prefixes == NULL) {
	return;
    }

    prefix = prefixes;
    while (*prefix != 0) {
	while (IS_BLANK(*prefix)) prefix++;
	if (*prefix == 0)
	    break;
        end = prefix;
	while ((*end != 0) && (!IS_BLANK(*end))) end++;
	prefix = xmlStrndup(prefix, end - prefix);
	if (prefix) {
	    xmlNsPtr ns;

	    if (xmlStrEqual(prefix, (const xmlChar *)"#default"))
		ns = xmlSearchNs(style->doc, cur, NULL);
	    else
		ns = xmlSearchNs(style->doc, cur, prefix);
	    if (ns == NULL) {
		xsltTransformError(NULL, style, cur,
	    "xsl:extension-element-prefix : undefined namespace %s\n",
	                         prefix);
		if (style != NULL) style->warnings++;
	    } else {
#ifdef WITH_XSLT_DEBUG_PARSING
		xsltGenericDebug(xsltGenericDebugContext,
		    "add extension prefix %s\n", prefix);
#endif
		xsltRegisterExtPrefix(style, prefix, ns->href);
	    }
	    xmlFree(prefix);
	}
	prefix = end;
    }
    xmlFree(prefixes);
}