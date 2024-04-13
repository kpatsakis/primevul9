xsltParseSimplifiedStylesheetTree(xsltCompilerCtxtPtr cctxt,
				  xmlDocPtr doc,
				  xmlNodePtr node)
{
    xsltTemplatePtr templ;
    
    if ((cctxt == NULL) || (node == NULL))
	return(-1);

    if (xsltParseAttrXSLTVersion(cctxt, node, 0) == XSLT_ELEMENT_CATEGORY_LRE)
    {
	/*
	* TODO: Adjust report, since this might be an
	* embedded stylesheet.
	*/
	xsltTransformError(NULL, cctxt->style, node,
	    "The attribute 'xsl:version' is missing; cannot identify "
	    "this document as an XSLT stylesheet document.\n");
	cctxt->style->errors++;
	return(1);
    }
    
#ifdef WITH_XSLT_DEBUG_PARSING
    xsltGenericDebug(xsltGenericDebugContext,
	"xsltParseSimplifiedStylesheetTree: document is stylesheet\n");
#endif        
    
    /*
    * Create and link the template
    */
    templ = xsltNewTemplate();
    if (templ == NULL) {
	return(-1);
    }
    templ->next = cctxt->style->templates;
    cctxt->style->templates = templ;
    templ->match = xmlStrdup(BAD_CAST "/");

    /*
    * Note that we push the document-node in this special case.
    */
    xsltCompilerNodePush(cctxt, (xmlNodePtr) doc);
    /*
    * In every case, we need to have
    * the in-scope namespaces of the element, where the
    * stylesheet is rooted at, regardless if it's an XSLT
    * instruction or a literal result instruction (or if
    * this is an embedded stylesheet).
    */
    cctxt->inode->inScopeNs =
	xsltCompilerBuildInScopeNsList(cctxt, node);
    /*
    * Parse the content and register the match-pattern.
    */
    xsltParseSequenceConstructor(cctxt, node);
    xsltCompilerNodePop(cctxt, (xmlNodePtr) doc);

    templ->elem = (xmlNodePtr) doc;
    templ->content = node;
    xsltAddTemplate(cctxt->style, templ, NULL, NULL);
    cctxt->style->literal_result = 1;
    return(0);
}