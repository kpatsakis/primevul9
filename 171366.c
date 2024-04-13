xsltParseXSLTTemplate(xsltCompilerCtxtPtr cctxt, xmlNodePtr templNode) {
    xsltTemplatePtr templ;
    xmlChar *prop;    
    double  priority;    

    if ((cctxt == NULL) || (templNode == NULL))
	return;

    /*
     * Create and link the structure
     */
    templ = xsltNewTemplate();
    if (templ == NULL)
	return;

    xsltCompilerNodePush(cctxt, templNode);
    if (templNode->nsDef != NULL)
	cctxt->inode->inScopeNs =
	    xsltCompilerBuildInScopeNsList(cctxt, templNode);

    templ->next = cctxt->style->templates;
    cctxt->style->templates = templ;
    templ->style = cctxt->style;  

    /*
    * Attribute "mode".
    */
    prop = xmlGetNsProp(templNode, (const xmlChar *)"mode", NULL);
    if (prop != NULL) {	
        const xmlChar *modeURI;

	/*
	* TODO: We need a standardized function for extraction
	*  of namespace names and local names from QNames.
	*  Don't use xsltGetQNameURI() as it cannot channe
	*  reports through the context.
	*/
	modeURI = xsltGetQNameURI(templNode, &prop);
	if (prop == NULL) {
	    cctxt->style->errors++;
	    goto error;
	}
	templ->mode = xmlDictLookup(cctxt->style->dict, prop, -1);
	xmlFree(prop);
	prop = NULL;
	if (xmlValidateNCName(templ->mode, 0)) {
	    xsltTransformError(NULL, cctxt->style, templNode,
		"xsl:template: Attribute 'mode': The local part '%s' "
		"of the value is not a valid NCName.\n", templ->name);
	    cctxt->style->errors++;
	    goto error;
	}
	if (modeURI != NULL)
	    templ->modeURI = xmlDictLookup(cctxt->style->dict, modeURI, -1);
#ifdef WITH_XSLT_DEBUG_PARSING
	xsltGenericDebug(xsltGenericDebugContext,
	     "xsltParseXSLTTemplate: mode %s\n", templ->mode);
#endif
    }
    /*
    * Attribute "match".
    */
    prop = xmlGetNsProp(templNode, (const xmlChar *)"match", NULL);
    if (prop != NULL) {
	templ->match  = prop;
	prop = NULL;
    }
    /*
    * Attribute "priority".
    */
    prop = xmlGetNsProp(templNode, (const xmlChar *)"priority", NULL);
    if (prop != NULL) {
	priority = xmlXPathStringEvalNumber(prop);
	templ->priority = (float) priority;
	xmlFree(prop);
	prop = NULL;
    }
    /*
    * Attribute "name".
    */
    prop = xmlGetNsProp(templNode, (const xmlChar *)"name", NULL);
    if (prop != NULL) {
        const xmlChar *nameURI;
	xsltTemplatePtr curTempl;
	
	/*
	* TODO: Don't use xsltGetQNameURI().
	*/
	nameURI = xsltGetQNameURI(templNode, &prop);
	if (prop == NULL) {
	    cctxt->style->errors++;
	    goto error;
	}
	templ->name = xmlDictLookup(cctxt->style->dict, prop, -1);
	xmlFree(prop);
	prop = NULL;
	if (xmlValidateNCName(templ->name, 0)) {
	    xsltTransformError(NULL, cctxt->style, templNode,
		"xsl:template: Attribute 'name': The local part '%s' of "
		"the value is not a valid NCName.\n", templ->name);
	    cctxt->style->errors++;
	    goto error;
	} 	
	if (nameURI != NULL)
	    templ->nameURI = xmlDictLookup(cctxt->style->dict, nameURI, -1);
	curTempl = templ->next;
	while (curTempl != NULL) {
	    if ((nameURI != NULL && xmlStrEqual(curTempl->name, templ->name) &&
		xmlStrEqual(curTempl->nameURI, nameURI) ) ||
		(nameURI == NULL && curTempl->nameURI == NULL &&
		xmlStrEqual(curTempl->name, templ->name)))
	    {
		xsltTransformError(NULL, cctxt->style, templNode,
		    "xsl:template: error duplicate name '%s'\n", templ->name);
		cctxt->style->errors++;
		goto error;
	    }
	    curTempl = curTempl->next;
	}
    }
    if (templNode->children != NULL) {
	xsltParseTemplateContent(cctxt->style, templNode);	
	/*
	* MAYBE TODO: Custom behaviour: In order to stay compatible with
	* Xalan and MSXML(.NET), we could allow whitespace
	* to appear before an xml:param element; this whitespace
	* will additionally become part of the "template".
	* NOTE that this is totally deviates from the spec, but
	* is the de facto behaviour of Xalan and MSXML(.NET).
	* Personally I wouldn't allow this, since if we have:
	* <xsl:template ...xml:space="preserve">
	*   <xsl:param name="foo"/>
	*   <xsl:param name="bar"/>
	*   <xsl:param name="zoo"/>
	* ... the whitespace between every xsl:param would be
	* added to the result tree.
	*/		
    }    
    
    templ->elem = templNode;
    templ->content = templNode->children;
    xsltAddTemplate(cctxt->style, templ, templ->mode, templ->modeURI);

error:
    xsltCompilerNodePop(cctxt, templNode);
    return;
}