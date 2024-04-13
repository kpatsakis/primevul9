xsltParseStylesheetTemplate(xsltStylesheetPtr style, xmlNodePtr template) {
    xsltTemplatePtr ret;
    xmlChar *prop;
    xmlChar *mode = NULL;
    xmlChar *modeURI = NULL;
    double  priority;

    if (template == NULL)
	return;

    /*
     * Create and link the structure
     */
    ret = xsltNewTemplate();
    if (ret == NULL)
	return;
    ret->next = style->templates;
    style->templates = ret;
    ret->style = style;
   
    /*
     * Get inherited namespaces
     */
    /*
    * TODO: Apply the optimized in-scope-namespace mechanism
    *   as for the other XSLT instructions.
    */
    xsltGetInheritedNsList(style, ret, template);

    /*
     * Get arguments
     */
    prop = xmlGetNsProp(template, (const xmlChar *)"mode", NULL);
    if (prop != NULL) {
        const xmlChar *URI;

	/*
	* TODO: Don't use xsltGetQNameURI().
	*/
	URI = xsltGetQNameURI(template, &prop);
	if (prop == NULL) {
	    if (style != NULL) style->errors++;
	    goto error;
	} else {
	    mode = prop;
	    if (URI != NULL)
		modeURI = xmlStrdup(URI);
	}
	ret->mode = xmlDictLookup(style->dict, mode, -1);
	ret->modeURI = xmlDictLookup(style->dict, modeURI, -1);
#ifdef WITH_XSLT_DEBUG_PARSING
	xsltGenericDebug(xsltGenericDebugContext,
	     "xsltParseStylesheetTemplate: mode %s\n", mode);
#endif
        if (mode != NULL) xmlFree(mode);
	if (modeURI != NULL) xmlFree(modeURI);
    }
    prop = xmlGetNsProp(template, (const xmlChar *)"match", NULL);
    if (prop != NULL) {
	if (ret->match != NULL) xmlFree(ret->match);
	ret->match  = prop;
    }

    prop = xmlGetNsProp(template, (const xmlChar *)"priority", NULL);
    if (prop != NULL) {
	priority = xmlXPathStringEvalNumber(prop);
	ret->priority = (float) priority;
	xmlFree(prop);
    }

    prop = xmlGetNsProp(template, (const xmlChar *)"name", NULL);
    if (prop != NULL) {
        const xmlChar *URI;
	xsltTemplatePtr cur;
	
	/*
	* TODO: Don't use xsltGetQNameURI().
	*/
	URI = xsltGetQNameURI(template, &prop);
	if (prop == NULL) {
	    if (style != NULL) style->errors++;
	    goto error;
	} else {
	    if (xmlValidateNCName(prop,0)) {
	        xsltTransformError(NULL, style, template,
	            "xsl:template : error invalid name '%s'\n", prop);
		if (style != NULL) style->errors++;
		goto error;
	    }
	    ret->name = xmlDictLookup(style->dict, BAD_CAST prop, -1);
	    xmlFree(prop);
	    prop = NULL;
	    if (URI != NULL)
		ret->nameURI = xmlDictLookup(style->dict, BAD_CAST URI, -1);
	    else
		ret->nameURI = NULL;
	    cur = ret->next;
	    while (cur != NULL) {
	        if ((URI != NULL && xmlStrEqual(cur->name, ret->name) &&
				xmlStrEqual(cur->nameURI, URI) ) ||
		    (URI == NULL && cur->nameURI == NULL &&
				xmlStrEqual(cur->name, ret->name))) {
		    xsltTransformError(NULL, style, template,
		        "xsl:template: error duplicate name '%s'\n", ret->name);
		    style->errors++;
		    goto error;
		}
		cur = cur->next;
	    }
	}
    }

    /*
     * parse the content and register the pattern
     */
    xsltParseTemplateContent(style, template);
    ret->elem = template;
    ret->content = template->children;
    xsltAddTemplate(style, ret, ret->mode, ret->modeURI);

error:
    return;
}