xsltParseStylesheetExcludePrefix(xsltStylesheetPtr style, xmlNodePtr cur,
				 int isXsltElem)
{
    int nb = 0;
    xmlChar *prefixes;
    xmlChar *prefix, *end;

    if ((cur == NULL) || (style == NULL))
	return(0);

    if (isXsltElem)
	prefixes = xmlGetNsProp(cur,
	    (const xmlChar *)"exclude-result-prefixes", NULL);
    else
	prefixes = xmlGetNsProp(cur,
	    (const xmlChar *)"exclude-result-prefixes", XSLT_NAMESPACE);

    if (prefixes == NULL) {
	return(0);
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
	    "xsl:exclude-result-prefixes : undefined namespace %s\n",
	                         prefix);
		if (style != NULL) style->warnings++;
	    } else {
		if (exclPrefixPush(style, (xmlChar *) ns->href) >= 0) {
#ifdef WITH_XSLT_DEBUG_PARSING
		    xsltGenericDebug(xsltGenericDebugContext,
			"exclude result prefix %s\n", prefix);
#endif
		    nb++;
		}
	    }
	    xmlFree(prefix);
	}
	prefix = end;
    }
    xmlFree(prefixes);
    return(nb);
}