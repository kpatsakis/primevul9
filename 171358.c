xsltParseXSLTStylesheetElem(xsltCompilerCtxtPtr cctxt, xmlNodePtr node)
{
    xmlNodePtr cur, start;

    if ((cctxt == NULL) || (node == NULL))
	return(-1);
    
    if (node->children == NULL)
	goto exit;

    /*
    * Process top-level elements:
    *  xsl:import (must be first)
    *  xsl:include (this is just a pre-processing)
    */
    cur = node->children;
    /*
    * Process xsl:import elements.
    * XSLT 1.0: "The xsl:import element children must precede all
    *  other element children of an xsl:stylesheet element,
    *  including any xsl:include element children."
    */    
    while ((cur != NULL) &&
	xsltParseFindTopLevelElem(cctxt, cur,
	    BAD_CAST "import", XSLT_NAMESPACE, 1, &cur) == 1)
    {
	if (xsltParseStylesheetImport(cctxt->style, cur) != 0) {
	    cctxt->style->errors++;
	}
	cur = cur->next;
    }
    if (cur == NULL)
	goto exit;
    start = cur;
    /*
    * Pre-process all xsl:include elements.
    */
    cur = start;
    while ((cur != NULL) &&
	xsltParseFindTopLevelElem(cctxt, cur,
	    BAD_CAST "include", XSLT_NAMESPACE, 0, &cur) == 1)
    {
	xsltParseTopLevelXSLTElem(cctxt, cur, XSLT_FUNC_INCLUDE);
	cur = cur->next;
    }
    /*
    * Pre-process all xsl:namespace-alias elements.
    * URGENT TODO: This won't work correctly: the order of included
    *  aliases and aliases defined here is significant.
    */
    cur = start;
    while ((cur != NULL) &&
	xsltParseFindTopLevelElem(cctxt, cur,
	    BAD_CAST "namespace-alias", XSLT_NAMESPACE, 0, &cur) == 1)
    {
	xsltNamespaceAlias(cctxt->style, cur);
	cur = cur->next;
    }

    if (cctxt->isInclude) {
	/*
	* If this stylesheet is intended for inclusion, then
	* we will process only imports and includes. 
	*/
	goto exit;
    } 
    /*
    * Now parse the rest of the top-level elements.
    */
    xsltParseXSLTStylesheetElemCore(cctxt, node); 	
exit:

    return(0);
}