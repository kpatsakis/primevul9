xsltParseStylesheetProcess(xsltStylesheetPtr style, xmlDocPtr doc)
{
    xsltCompilerCtxtPtr cctxt;
    xmlNodePtr cur;
    int oldIsSimplifiedStylesheet;

    xsltInitGlobals();

    if ((style == NULL) || (doc == NULL))
	return(NULL);

    cctxt = XSLT_CCTXT(style);

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
	xsltTransformError(NULL, style, (xmlNodePtr) doc,
		"xsltParseStylesheetProcess : empty stylesheet\n");
	return(NULL);
    }
    oldIsSimplifiedStylesheet = cctxt->simplified;

    if ((IS_XSLT_ELEM(cur)) && 
	((IS_XSLT_NAME(cur, "stylesheet")) ||
	 (IS_XSLT_NAME(cur, "transform")))) {	
#ifdef WITH_XSLT_DEBUG_PARSING
	xsltGenericDebug(xsltGenericDebugContext,
		"xsltParseStylesheetProcess : found stylesheet\n");
#endif
	cctxt->simplified = 0;
	style->literal_result = 0;
    } else {
	cctxt->simplified = 1;
	style->literal_result = 1;
    }
    /*
    * Pre-process the stylesheet if not already done before.
    *  This will remove PIs and comments, merge adjacent
    *  text nodes, internalize strings, etc.
    */
    if (! style->nopreproc)
	xsltParsePreprocessStylesheetTree(cctxt, cur);
    /*
    * Parse and compile the stylesheet.
    */
    if (style->literal_result == 0) {
	if (xsltParseXSLTStylesheetElem(cctxt, cur) != 0)
	    return(NULL);
    } else {
	if (xsltParseSimplifiedStylesheetTree(cctxt, doc, cur) != 0)
	    return(NULL);
    }    

    cctxt->simplified = oldIsSimplifiedStylesheet;

    return(style);
}