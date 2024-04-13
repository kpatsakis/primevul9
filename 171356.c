xsltParseStylesheetProcess(xsltStylesheetPtr ret, xmlDocPtr doc) {
    xmlNodePtr cur;

    xsltInitGlobals();

    if (doc == NULL)
	return(NULL);
    if (ret == NULL)
	return(ret);
    
    /*
     * First steps, remove blank nodes,
     * locate the xsl:stylesheet element and the
     * namespace declaration.
     */
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
	xsltTransformError(NULL, ret, (xmlNodePtr) doc,
		"xsltParseStylesheetProcess : empty stylesheet\n");
	return(NULL);
    }

    if ((IS_XSLT_ELEM(cur)) && 
	((IS_XSLT_NAME(cur, "stylesheet")) ||
	 (IS_XSLT_NAME(cur, "transform")))) {	
#ifdef WITH_XSLT_DEBUG_PARSING
	xsltGenericDebug(xsltGenericDebugContext,
		"xsltParseStylesheetProcess : found stylesheet\n");
#endif
	ret->literal_result = 0;
	xsltParseStylesheetExcludePrefix(ret, cur, 1);
	xsltParseStylesheetExtPrefix(ret, cur, 1);
    } else {
	xsltParseStylesheetExcludePrefix(ret, cur, 0);
	xsltParseStylesheetExtPrefix(ret, cur, 0);
	ret->literal_result = 1;
    }
    if (!ret->nopreproc) {
	xsltPrecomputeStylesheet(ret, cur);
    }
    if (ret->literal_result == 0) {
	xsltParseStylesheetTop(ret, cur);
    } else {
	xmlChar *prop;
	xsltTemplatePtr template;

	/*
	 * the document itself might be the template, check xsl:version
	 */
	prop = xmlGetNsProp(cur, (const xmlChar *)"version", XSLT_NAMESPACE);
	if (prop == NULL) {
	    xsltTransformError(NULL, ret, cur,
		"xsltParseStylesheetProcess : document is not a stylesheet\n");
	    return(NULL);
	}

#ifdef WITH_XSLT_DEBUG_PARSING
        xsltGenericDebug(xsltGenericDebugContext,
		"xsltParseStylesheetProcess : document is stylesheet\n");
#endif
	
	if (!xmlStrEqual(prop, (const xmlChar *)"1.0")) {
	    xsltTransformError(NULL, ret, cur,
		"xsl:version: only 1.0 features are supported\n");
	     /* TODO set up compatibility when not XSLT 1.0 */
	    ret->warnings++;
	}
	xmlFree(prop);

	/*
	 * Create and link the template
	 */
	template = xsltNewTemplate();
	if (template == NULL) {
	    return(NULL);
	}
	template->next = ret->templates;
	ret->templates = template;
	template->match = xmlStrdup((const xmlChar *)"/");

	/*
	 * parse the content and register the pattern
	 */
	xsltParseTemplateContent(ret, (xmlNodePtr) doc);
	template->elem = (xmlNodePtr) doc;
	template->content = doc->children;
	xsltAddTemplate(ret, template, NULL, NULL);
	ret->literal_result = 1;	
    }

    return(ret);
}