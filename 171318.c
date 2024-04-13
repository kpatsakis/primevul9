xsltParseNsPrefixList(xsltCompilerCtxtPtr cctxt,
		      xsltPointerListPtr list,
		      xmlNodePtr node,
		      const xmlChar *value)
{
    xmlChar *cur, *end;
    xmlNsPtr ns;
    
    if ((cctxt == NULL) || (value == NULL) || (list == NULL))
	return(-1);

    list->number = 0;

    cur = (xmlChar *) value;
    while (*cur != 0) {
	while (IS_BLANK(*cur)) cur++;
	if (*cur == 0)
	    break;
	end = cur;
	while ((*end != 0) && (!IS_BLANK(*end))) end++;
	cur = xmlStrndup(cur, end - cur);
	if (cur == NULL) {
	    cur = end;
	    continue;
	}		
	/*
	* TODO: Export and use xmlSearchNsByPrefixStrict()
	*   in Libxml2, tree.c, since xmlSearchNs() is in most
	*   cases not efficient and in some cases not correct.
	*
	* XSLT-2 TODO: XSLT 2.0 allows an additional "#all" value.
	*/
	if ((cur[0] == '#') &&
	    xmlStrEqual(cur, (const xmlChar *)"#default"))
	    ns = xmlSearchNs(cctxt->style->doc, node, NULL);
	else
	    ns = xmlSearchNs(cctxt->style->doc, node, cur);	    

	if (ns == NULL) {
	    /*
	    * TODO: Better to report the attr-node, otherwise
	    *  the user won't know which attribute was invalid.
	    */
	    xsltTransformError(NULL, cctxt->style, node,
		"No namespace binding in scope for prefix '%s'.\n", cur);
	    /*
	    * XSLT-1.0: "It is an error if there is no namespace
	    *  bound to the prefix on the element bearing the
	    *  exclude-result-prefixes or xsl:exclude-result-prefixes
	    *  attribute."
	    */
	    cctxt->style->errors++;
	} else {
#ifdef WITH_XSLT_DEBUG_PARSING
	    xsltGenericDebug(xsltGenericDebugContext,
		"resolved prefix '%s'\n", cur);
#endif
	    /*
	    * Note that we put the namespace name into the dict.
	    */
	    if (xsltPointerListAddSize(list,
		(void *) xmlDictLookup(cctxt->style->dict,
		ns->href, -1), 5) == -1)
	    {
		xmlFree(cur);
		goto internal_err;
	    }
	}
	xmlFree(cur);
		
	cur = end;
    }
    return(0);

internal_err:
    cctxt->style->errors++;
    return(-1);
}